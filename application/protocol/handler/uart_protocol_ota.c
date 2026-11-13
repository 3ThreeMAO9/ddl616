/**
 * Copyright (c) 2026 GZ-OB, All rights reserved.
 * File name: uart_protocol_ota.c
 * Desc: 串口 OTA 升级（模组 → 门锁）
 *
 * 流程：
 *   0x72 开始   → 0xF2(status + pkt_size)，同时擦除 SPI flash 的 64K OTA 区
 *   0x73 数据   → 0xF3(status + expected)，逐包写 flash 并回读比对
 *   0x73 零长包 → 0xF3，校验失败时立即上报 0x74
 *   校验通过    → 置 ota_info=WAIT，200ms 后跳 boot
 *   boot 搬运   → 跳回新 app，app 启动后上报 0x74 成功 + 新版本号
 *
 * 关键约束：
 *   - 下载方向（0x72/0x73）明文；0x74 由门锁用会话密钥加密上报
 *   - 整帧 = 12(头) + 4(offset) + OTA_PKT_SIZE，须小于 UART0_BUF_LEN
 *   - 固件先落 SPI flash，再由 boot 搬到 FMC
 *
 * 协议细节见 doc/串口OTA升级对接说明_v1.2.md
 * Version 1.4.1
 * Date: 2026-11-13
 */

#if (OTA_ENABLE == 1)   /* OTA 整体开关：未启用时本文件编译为空，boot_symbol 依赖随之消失 */

#include "uart_protocol.h"
#include "msg_protocol.h"
#include "module_spi_flash.h"
#include "chip_config.h"
#include "ota_info.h"
#include "ota_file.h"
#include "hal_flash.h"
#include "sfud.h"
#include "hal_wdt.h"
#include "item_config.h"
#include "system_timer.h"
#include <string.h>

#define OB_LOG_LEVEL OB_LOG_LEVEL_NONE
#include "ob_log.h"
#define TAG "uart_ota"

typedef struct {
    uint8_t  active;        /* 1 = 正在接收固件 */
    uint8_t  version[3];
    uint32_t total;         /* 对端声明的固件长度 */
    uint32_t expected;      /* 下一个包的偏移，也是已写入字节数 */
    uint32_t checksum;      /* 对端给的 32 位累加和 */
    uint32_t calc;          /* 本地边收边算的累加和 */
    uint32_t last_ms;       /* 最近一次收到 0x73 的时刻，用于会话超时 */
} ota_ctx_t;

static ota_ctx_t s_ota;

/* 会话超时：这段时间没收到任何 0x73 就视为对端已死，清掉悬挂会话，
 * 让下一次 0x72 能重新开始（模组复位/串口中断后无需设备断电）。
 * 整包下载约 10 秒，单包重试链路最长约 15 秒，2 分钟不会误伤正常会话 */
#define OTA_SESSION_TIME_OUT_MS     (120u * 1000u)

/* boot 入口地址由 common/boot_symbol.obj 固定，切 MSP 后直接跳转 */
__ASM void go_boot(void)
{
    IMPORT boot_main

    MOVS  R0, #0
    LDR   R1, [R0]
    MSR   MSP, R1
    LDR   R1, =boot_main
    BX    R1
}

static sfud_flash *ota_flash(void)
{
    return sfud_get_device(0);      /* OTA 用 sfud 设备表的第 0 个 SPI flash */
}

/* OTA 区地址：优先用语音包头声明的 OTA 域（域起始 + 域大小/2，即后半区）；
 * 没有声明时退回「容量末尾 64K」，与 boot 的 SFLASH_OTA_BEGIN_ADDR 一致。
 * 按实际容量计算，换 flash 或改分区布局时 app 自动跟随。 */
static uint32_t ota_area_addr(void)
{
    uint32_t base = read_ota_flash_address();
    uint32_t size = read_ota_flash_size();
    sfud_flash *flash = ota_flash();

    if (base && size)
        return base + (size / 2u) * 1024u;      /* ota_size 单位 KB，域被对半分 */

    return (flash == NULL) ? 0 : (flash->chip.capacity - OTA_FLASH_MAX_SIZE);
}

/* 32 位累加和：4 字节一组小端累加，尾部不足 4 字节补零，
 * 与 ota_file_calc_checksum_u32 同口径（只覆盖数据段，不含 16 字节文件头） */
static uint32_t ota_checksum_add(uint32_t sum, const uint8_t *p, uint32_t size)
{
    while (size >= 4)
    {
        sum += (uint32_t)p[0]
             | ((uint32_t)p[1] << 8)
             | ((uint32_t)p[2] << 16)
             | ((uint32_t)p[3] << 24);
        p += 4;
        size -= 4;
    }

    if (size)
    {
        uint32_t tail = 0;
        uint32_t shift = 0;
        while (size--)
        {
            tail |= (uint32_t)(*p++) << shift;
            shift += 8;
        }
        sum += tail;
    }

    return sum;
}

/* 结束包后回读镜像头，与本次下发的期望值逐项比对：
 * magic / size / checksum2 / version。
 * sfud 写失败会静默返回 SFUD_SUCCESS，只有回读才可信；
 * 必须比对期望值而不能只做自洽校验 —— flash 里残留的旧镜像也是自洽的。
 * 返回 0 = 通过。 */
static uint32_t ota_check_flash(void)
{
    ota_file_t hdr;
    uint32_t base = ota_area_addr();

    /* sfud_write 返回前已在内部轮询 WIP 等写完成，无需额外延时 */
    if (hal_flash_read(base, &hdr, sizeof(hdr)) != 0)
        return 1;

    if (hdr.magic != OTA_FILE_MAGIC)
    {
        OB_LOGE(TAG, "readback magic=%04X", hdr.magic);
        return 1;
    }

    if (hdr.size != (s_ota.total - sizeof(ota_file_t)))
    {
        OB_LOGE(TAG, "readback size=%u expect=%u",
                hdr.size, s_ota.total - (uint32_t)sizeof(ota_file_t));
        return 2;
    }

    if (hdr.checksum2 != s_ota.calc)
    {
        OB_LOGE(TAG, "readback csum=%08X expect=%08X", hdr.checksum2, s_ota.calc);
        return 2;
    }

    if ((hdr.version[0] != s_ota.version[0])
            || (hdr.version[1] != s_ota.version[1])
            || (hdr.version[2] != s_ota.version[2]))
    {
        OB_LOGE(TAG, "readback ver=%u.%u.%u expect=%u.%u.%u",
                hdr.version[0], hdr.version[1], hdr.version[2],
                s_ota.version[0], s_ota.version[1], s_ota.version[2]);
        return 2;
    }

    return 0;
}

#define OTA_CHECK_BUF_SIZE      (64)
/* 每写完一包立刻回读比对，确保真的落盘。
 * sfud_write 只负责发命令，写保护/时序问题时会静默失败并返回 SFUD_SUCCESS，
 * 只有回读才能确认。按 64 字节分段回读：栈占用小，且不受单包长度限制。
 * 返回 0 = 一致。 */
static uint32_t ota_verify_write(uint32_t addr, const uint8_t *src, uint16_t len)
{
    uint8_t check_buf[OTA_CHECK_BUF_SIZE];
    sfud_flash *flash = ota_flash();
    uint16_t done = 0;

    if (flash == NULL)
        return 1;

    while (done < len)
    {
        uint16_t chunk = (uint16_t)(len - done);
        uint16_t i;

        if (chunk > OTA_CHECK_BUF_SIZE)
            chunk = OTA_CHECK_BUF_SIZE;

        if (hal_flash_read(addr + done, check_buf, chunk) != 0)
        {
            OB_LOGE(TAG, "w 0x%05X read fail", addr + done);
            return 1;
        }

        for (i = 0; i < chunk; i++)
        {
            if (check_buf[i] != src[done + i])
            {
                OB_LOGE(TAG, "w 0x%05X mis @%u %02X!=%02X",
                        addr + done, i, check_buf[i], src[done + i]);
                return 1;
            }
        }

        done = (uint16_t)(done + chunk);
    }

    OB_LOGD(TAG, "w 0x%05X %u ok", addr, len);
    return 0;
}

/* 0x74 结果上报：[result 1B][version 3B]
 *
 * version 用本机真实版本，编码与镜像头、uart_msg_front_version_str() 一致
 * （镜像头的 0.0.1 已在打包时被 make_fw.py 换成真实版本）：
 *   sn = 39140 → [39][140]，PATCH_VERSION → 最后一字节
 *   例：成功 + V39.140.005 → [00][39][140][05] */
static void ota_send_result(uint8_t result)
{
    uint16_t sn = readDeviceClientProjectSn();
    uint16_t pv = readDeviceVersion();
    uint8_t buf[4];

    buf[0] = result;
    buf[1] = (uint8_t)((sn / 1000) % 1000);   /* 39  */
    buf[2] = (uint8_t)(sn % 1000);            /* 140 */
    buf[3] = (uint8_t)pv;                     /* 5   */

    uart_msg_property_report(UP_CMD_OTA_RESULT, buf, sizeof(buf));
}

/* ================= 0x72 升级开始 =================
 * 校验长度/容量 → 清写保护 → 擦除 64K OTA 区 → 回 0xF2(status + pkt_size)。
 * 擦除耗时 1~2 秒，期间不应答属正常。
 * 会话 2 分钟没收到 0x73 自动作废（uart_ota_session_poll），模组恢复后
 * 发零长结束包清会话再重发 0x72，或等超时后直接重发 0x72。 */
HANDLER_DEFINE(UP_CMD_OTA_START)
{
    uint8_t ack[3] = { OTA_START_STATUS_NO_SPACE, 0, 0 };
    sfud_flash *flash = ota_flash();
    uint8_t *p = packet->payload;

    if ((packet->length < 11) || (flash == NULL))
    {
        uart_msg_property_ack(packet->cmd, ack, sizeof(ack));
        return 0;
    }

    /* 会话进行中拒绝重入（status=2）。会话 30 秒没收到 0x73 会被
     * uart_ota_session_poll 自动清除，之后 0x72 即可重新开始；
     * 模组也可发零长结束包强制清会话，不必断电。 */
    if (s_ota.active)
    {
        ack[0] = OTA_START_STATUS_BUSY;
        uart_msg_property_ack(packet->cmd, ack, sizeof(ack));
        return 0;
    }

    uint32_t size = (uint32_t)p[0] | ((uint32_t)p[1] << 8)
                  | ((uint32_t)p[2] << 16) | ((uint32_t)p[3] << 24);

    if ((size < 4) || (size > FLASH_APP_MAX_SIZE))
    {
        uart_msg_property_ack(packet->cmd, ack, sizeof(ack));
        return 0;
    }

    /* 版本与本地一致时不升级，避免同版本重复擦写、白白重启一次 */
    uint16_t sn = readDeviceClientProjectSn();
    if ((p[8] == (uint8_t)((sn / 1000) % 1000)) &&
        (p[9] == (uint8_t)(sn % 1000)) &&
        (p[10] == (uint8_t)readDeviceVersion()))
    {
        ack[0] = OTA_START_STATUS_SAME_VER;
        uart_msg_property_ack(packet->cmd, ack, sizeof(ack));
        OB_LOGI(TAG, "same version %u.%u.%u, skip ota", p[8], p[9], p[10]);
        return 0;
    }

    memset(&s_ota, 0, sizeof(s_ota));
    s_ota.total    = size;
    s_ota.checksum = (uint32_t)p[4] | ((uint32_t)p[5] << 8)
                   | ((uint32_t)p[6] << 16) | ((uint32_t)p[7] << 24);
    s_ota.version[0] = p[8];
    s_ota.version[1] = p[9];
    s_ota.version[2] = p[10];

    /* OTA 区地址：优先用语音包数据头里声明的 ota_address（域起始 + 域大小/2），
     * 没有声明时才退回「容量末尾 64K」。打印出来便于和 boot 对照。 */
    OB_LOGI(TAG, "ota addr=0x%05X", ota_area_addr());

    /* 擦除前清写保护。
     * boot 的 spi_flash_64k_erase / write 结尾都会 WRSR 0x7C 置 BP 位，
     * hal_flash_erase/write 内部只做 WREN，不会清 BP。
     * BP 位置上后芯片静默忽略擦写，hal_flash 仍返回成功。 */
    hal_flash_write_status(0x00);

    /* 擦除 64K，耗时 1~2 秒，期间无应答属正常 */
    if (hal_flash_erase(ota_area_addr(), OTA_FLASH_MAX_SIZE) != 0)
    {
        OB_LOGE(TAG, "erase fail");
        ack[0] = OTA_START_STATUS_ERASE_FAIL;
        uart_msg_property_ack(packet->cmd, ack, sizeof(ack));
        return 0;
    }

    s_ota.active = 1;
    s_ota.last_ms = system_ms_get();
    ack[0] = OTA_START_STATUS_READY;
    ack[1] = (uint8_t)(OTA_PKT_SIZE & 0xFF);
    ack[2] = (uint8_t)(OTA_PKT_SIZE >> 8);

    OB_LOGI(TAG, "ota start, size=%u", size);
    uart_msg_property_ack(packet->cmd, ack, sizeof(ack));

    return 0;
}

/* ================= 0x73 数据 / 结束包 =================
 * payload = [offset 4B][data N]，N == 0 表示结束包。
 * 结束包：校验长度与累加和 → 回读镜像头 → 回 0xF3；
 *         失败立即上报 0x74，成功不在这里报（等 boot 搬完由 app 上报）。 */
HANDLER_DEFINE(UP_CMD_OTA_DATA)
{
    uint8_t ack[5] = { OTA_DATA_STATUS_WRITE_FAIL, 0, 0, 0, 0 };
    uint8_t *p = packet->payload;
    uint32_t offset, ret = OTA_DATA_STATUS_OK;
    uint16_t len;

    /* flash 有效性已在 0x72 建会话时确认，这里无需再查 */
    if (packet->length < 4)
    {
        uart_msg_property_ack(packet->cmd, ack, sizeof(ack));
        return 0;
    }

    if (!s_ota.active)
    {
        ack[0] = OTA_DATA_STATUS_NO_SESSION;
        uart_msg_property_ack(packet->cmd, ack, sizeof(ack));
        return 0;
    }

    s_ota.last_ms = system_ms_get();        /* 对端还活着，刷新会话保活 */

    offset = (uint32_t)p[0] | ((uint32_t)p[1] << 8)
           | ((uint32_t)p[2] << 16) | ((uint32_t)p[3] << 24);
    len    = packet->length - 4;

    if (len == 0)
    {
        /* 结束包：校验长度与累加和 */
        if (s_ota.expected != s_ota.total)
        {
            ret = OTA_DATA_STATUS_OFFSET_ERROR;
            ota_send_result(OTA_RESULT_SIZE_FAIL);
        }
        else if (s_ota.calc != s_ota.checksum)
        {
            ret = OTA_DATA_STATUS_OFFSET_ERROR;
            ota_send_result(OTA_RESULT_CSUM_FAIL);
        }
        else
        {
            /* 回读镜像头，确认 flash 里确实是本次下发的镜像 */
            uint32_t chk = ota_check_flash();

            if (chk)
            {
                ret = OTA_DATA_STATUS_READBACK_FAIL;
                ota_send_result(OTA_RESULT_CSUM_FAIL);
            }
            /* 成功不在这里上报：下载完成只说明数据到了 SPI，
             * 真正完成是 boot 搬运后 app 启动时（uart_ota_report_boot_done） */
        }

        s_ota.active = 0;
    }
    else if (offset > s_ota.expected)
    {
        /* 跳包：回填期望偏移，让对端续传 */
        ret = OTA_DATA_STATUS_OFFSET_ERROR;
    }
    else if ((offset + len) <= s_ota.expected)
    {
        /* 重复包（NOR 不能重复编程），直接回成功，不再写 */
        ret = OTA_DATA_STATUS_OK;
    }
    else
    {
        /* 允许重传包与已写部分重叠，只补后面没写的部分 */
        uint32_t skip = s_ota.expected - offset;
        uint16_t wr   = (uint16_t)(len - skip);

        if ((s_ota.expected + wr) > s_ota.total)
            wr = (uint16_t)(s_ota.total - s_ota.expected);

        uint32_t waddr = ota_area_addr() + s_ota.expected;

        if ((hal_flash_write(waddr, p + 4 + skip, wr) != 0)
                || ota_verify_write(waddr, p + 4 + skip, wr))
        {
            ret = OTA_DATA_STATUS_WRITE_FAIL;   /* 写失败或回读不一致 */
        }

        /* 一旦写失败就清掉本次升级会话，避免 s_ota.active 卡在 1，
         * 下一次 0x72 被拒绝（status=2）永远重试不了 */
        if (ret == OTA_DATA_STATUS_WRITE_FAIL)
        {
            OB_LOGE(TAG, "write fail @0x%05X, abort", s_ota.expected);
            s_ota.active = 0;
        }
        else
        {
            /* 累加时跳过 16 字节文件头，使 s_ota.calc 与镜像头的 checksum2 同口径
             * （checksum2 只覆盖数据段，不含文件头） */
            {
                uint32_t head_skip = 0;

                if (s_ota.expected < sizeof(ota_file_t))
                    head_skip = sizeof(ota_file_t) - s_ota.expected;

                if (head_skip < wr)
                    s_ota.calc = ota_checksum_add(s_ota.calc,
                            p + 4 + skip + head_skip, wr - head_skip);
            }
            s_ota.expected += wr;
        }
    }

    /* 统一构建应答：[status][expected offset 4B] */
    ack[0] = (uint8_t)ret;
    ack[1] = (uint8_t)(s_ota.expected & 0xFF);
    ack[2] = (uint8_t)((s_ota.expected >> 8) & 0xFF);
    ack[3] = (uint8_t)((s_ota.expected >> 16) & 0xFF);
    ack[4] = (uint8_t)((s_ota.expected >> 24) & 0xFF);
    uart_msg_property_ack(packet->cmd, ack, sizeof(ack));

    /* 结束包成功：留出 200ms 让 0xF3 发完，由 uart_protocol_ota_poll 触发跳 boot */
    if ((len == 0) && (ret == OTA_DATA_STATUS_OK))
        uart_protocol_ota_to_boot_inc_time_out();

    return 0;
}

/* 会话超时检查：2 分钟没收到任何 0x73 视为对端已死（模组复位/串口中断），
 * 清掉悬挂的会话，下一次 0x72 即可重新开始，无需设备断电。
 * 由 uart_protocol_ota_poll 周期调用；无符号差值天然处理 ms 计数回绕。 */
void uart_ota_session_poll(void)
{
    if (s_ota.active &&
        (system_ms_get() - s_ota.last_ms) >= OTA_SESSION_TIME_OUT_MS)
    {
        memset(&s_ota, 0, sizeof(s_ota));
        OB_LOGI(TAG, "ota session timeout");
    }
}

/* 由 uart_protocol_ota_poll 轮询调用：boot 搬运完跳回 app 后，
 * ota_info.state == DONE 才算 OTA 真正完成，此时上报 0x74 成功 + 新版本；
 * 搬运失败时 state 仍是 WAIT 且 error != NONE，上报 0x74 失败。
 * 上报后状态复位为 IDLE，避免重复上报。
 * 返回 1 = 本次是 OTA 启动并已上报，0 = 普通启动。 */
uint32_t uart_ota_report_boot_done(void)
{
    ota_info_t *info;

    /* 正在接收新固件时不上报，避免把上一次遗留的 DONE 当成本次结果 */
    if (s_ota.active)
        return 0;

    info = ota_info_get(0);
    if (info->magic != OTA_INFO_MAGIC)
        return 0;

    if (info->state == OTA_INFO_STATE_DONE)
    {
        info->state = OTA_INFO_STATE_IDLE;
        info->error = OTA_INFO_ERROR_NONE;
        ota_info_update(info);
        ota_send_result(OTA_RESULT_SUCCESS);
        return 1;
    }

    /* boot 搬运失败：error 有值而 state 还停在 WAIT。
     * error==BKUP 表示备份阶段就失败、根本没搬镜像，旧固件还在跑 */
    if ((info->state == OTA_INFO_STATE_WAIT) && (info->error != OTA_INFO_ERROR_NONE))
    {
        info->state = OTA_INFO_STATE_IDLE;
        ota_info_update(info);

        ota_send_result((info->error == OTA_INFO_ERROR_BKUP)
                        ? OTA_RESULT_BACKUP_FAIL : OTA_RESULT_CSUM_FAIL);
        return 1;
    }
    return 0;
}

/* 写 ota_info（WAIT + error 清零）→ 关看门狗和中断 → 跳 boot。
 * 由 uart_protocol_ota_poll 在 0xF3 发出 200ms 后调用。 */
void uart_ota_jump_to_boot(void)
{
    ota_info_t *info = ota_info_get(0);

    info->error = OTA_INFO_ERROR_NONE;
    info->state = OTA_INFO_STATE_WAIT;
    ota_info_update(info);
    WDT_CLOSE();

    NVIC_DisableIRQ(SysTick_IRQn);
    IRQn_Type irq;
    for (irq = I2C1_IRQn; irq <= PWMTM_IRQn; irq++)
        NVIC_DisableIRQ(irq);

    go_boot();
}

#endif  /* OTA_ENABLE */
