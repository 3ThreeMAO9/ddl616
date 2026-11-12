/**
 * Copyright (c) 2026 GZ-OB, All rights reserved.
 * File name: uart_protocol_ota.c
 * Desc: 串口 OTA 升级：
 *       0x72 开始（要分包大小）→ 0x73 逐包推送 + 应答 → 零长包收尾 → 0x74 结果上报
 *       固件先落到 SPI flash 的 OTA 区，再由 boot 搬运到 FMC
 * Version 1.3.0
 * Date: 2026-11-09
 *
 * 协议细节见 doc/串口OTA升级对接说明_v1.1.md
 */

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
} ota_ctx_t;

static ota_ctx_t s_ota;

/* boot 入口，地址由 common/boot_symbol.obj 解析 */
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
    return sfud_get_device(0);
}

/* OTA 区在 SPI flash 末尾，按实际容量算，避免芯片容量与宏不一致 */
static uint32_t ota_area_addr(void)
{
    uint32_t base = read_ota_flash_address();
    uint32_t size = read_ota_flash_size();
    sfud_flash *flash = ota_flash();

    /* 语音包数据头给的是整块 OTA 域（含备份槽），域被对半分：
     * 前半备份、后半待升级。所以待升级区 = 域起始 + 域大小/2。
     * ota_size 单位是 KB（实测 128 → 128KB = 0x20000），
     * 0x3E0000 + 128/2*1024 = 0x3F0000，正好是 boot 的 SFLASH_OTA_BEGIN_ADDR。
     * 这样换 flash 或改布局时只要语音包头下发对了，app 自动跟随。 */
    if (base && size)
        return base + (size / 2u) * 1024u;

    /* 语音包头不可用时退回「容量末尾 64K」，与 boot 的 SFLASH_OTA_BEGIN_ADDR 一致 */
    return (flash == NULL) ? 0 : (flash->chip.capacity - OTA_FLASH_MAX_SIZE);
}

/* 32 位累加和，与 ota_file_calc_checksum_u32 一致（4 字节小端累加，尾部补零） */
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

/* 回读 SPI flash 的镜像头，和本次 OTA 的期望值逐项比对。
 * 对应参考例程 ota_helper_check_data()：sfud 的写不做任何校验，
 * 写保护/时序问题时依然返回 SFUD_SUCCESS，只有回读才能发现。
 *
 * 逐包写入时已经由 ota_verify_write() 逐字节比对过，这里只确认头部：
 * magic / size / checksum2 / 版本。必须比对期望值而不是只校验自洽 ——
 * flash 里若残留上一次的镜像，它自己也是自洽的。
 * 返回 0 = 通过。 */
static uint32_t ota_check_flash(void)
{
    ota_file_t hdr;
    uint32_t base = ota_area_addr();

    /* 最后一个包的写入可能还在芯片内部进行，稍等一下再读 */
    for (volatile uint32_t d = 0; d < 20000u; d++)
        ;

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

/* 0x0C 结果上报：[result 1B][version 3B]
 *
 * version 填本机真实版本，不用镜像头里那个 0.0.1（那是 packer -v 写死的
 * 常量，跟 PATCH_VERSION 无关，无法用来判断版本有没有翻转）。
 * 编码与 uart_msg_front_version_str() 一致：
 *   sn = 39140 → [39][140]，PATCH_VERSION → 最后一字节（5） */
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

/* 0x72：升级开始 */
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
        ack[0] = OTA_START_STATUS_BUSY;
        uart_msg_property_ack(packet->cmd, ack, sizeof(ack));
        return 0;
    }

    s_ota.active = 1;
    ack[0] = OTA_START_STATUS_READY;
    ack[1] = (uint8_t)(OTA_PKT_SIZE & 0xFF);
    ack[2] = (uint8_t)(OTA_PKT_SIZE >> 8);

    OB_LOGI(TAG, "ota start, size=%u", size);
    uart_msg_property_ack(packet->cmd, ack, sizeof(ack));

    return 0;
}

/* 0x73：数据包 / 结束包（data 长度为 0 表示结束） */
HANDLER_DEFINE(UP_CMD_OTA_DATA)
{
    uint8_t ack[5] = { OTA_DATA_STATUS_WRITE_FAIL, 0, 0, 0, 0 };
    sfud_flash *flash = ota_flash();
    uint8_t *p = packet->payload;
    uint32_t offset, ret = OTA_DATA_STATUS_OK;
    uint16_t len;

    if ((packet->length < 4) || (!s_ota.active) || (flash == NULL))
    {
        uart_msg_property_ack(packet->cmd, ack, sizeof(ack));
        return 0;
    }

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
            /* 回读校验：sfud 的写不校验，写保护/时序问题也会返回成功，
             * 只有回读才能发现「报了成功但 flash 里还是旧镜像」。
             * 参考例程 ota_timer.c 也是校验通过才允许重启。 */
            uint32_t chk = ota_check_flash();

            if (chk)
            {
                ret = OTA_DATA_STATUS_READBACK_FAIL;
                ota_send_result(OTA_RESULT_CSUM_FAIL);
            }
            /* 成功不在这里报！下载完成只说明数据到了 SPI，
             * 真正的 OTA 完成是 boot 把镜像搬到 FMC、跳回 app 之后，
             * 由 uart_ota_report_boot_done() 上报。*/
        }

        s_ota.active = 0;

        ack[0] = (uint8_t)ret;
        ack[1] = (uint8_t)(s_ota.expected & 0xFF);
        ack[2] = (uint8_t)((s_ota.expected >> 8) & 0xFF);
        ack[3] = (uint8_t)((s_ota.expected >> 16) & 0xFF);
        ack[4] = (uint8_t)((s_ota.expected >> 24) & 0xFF);
        uart_msg_property_ack(packet->cmd, ack, sizeof(ack));

        if (ret == OTA_DATA_STATUS_OK)
        {
            /* 留出时间把应答和结果发出去，由 uart_protocol_ota_poll 触发跳转 */
            uart_protocol_ota_to_boot_inc_time_out();
        }
        return 0;
    }

    if (offset > s_ota.expected)
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

        if (hal_flash_write(ota_area_addr() + s_ota.expected,
                p + 4 + skip, wr) != 0)
        {
            ret = OTA_DATA_STATUS_WRITE_FAIL;
        }
        else if (ota_verify_write(ota_area_addr() + s_ota.expected,
                    p + 4 + skip, wr))
        {
            ret = OTA_DATA_STATUS_WRITE_FAIL;   /* 回读不一致 */
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

    ack[0] = (uint8_t)ret;
    ack[1] = (uint8_t)(s_ota.expected & 0xFF);
    ack[2] = (uint8_t)((s_ota.expected >> 8) & 0xFF);
    ack[3] = (uint8_t)((s_ota.expected >> 16) & 0xFF);
    ack[4] = (uint8_t)((s_ota.expected >> 24) & 0xFF);

    uart_msg_property_ack(packet->cmd, ack, sizeof(ack));
    return 0;
}

/* 由 uart_protocol_ota_poll 轮询调用：boot 把镜像搬到 FMC 并跳回 app 后，ota_info.state == DONE，
 * 这才是 OTA 真正完成的时刻 —— 下载结束只说明数据到了 SPI，
 * boot 搬运失败的话版本不会变，那时上报成功是骗人。
 *
 * 状态由 boot 在搬运结束后写入（OTA_INFO_STATE_DONE）；
 * 搬运失败时 error != NONE 而 state 仍是 WAIT。
 * 上报后把 state 复位成 IDLE，避免下次轮询重复上报。
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

    /* boot 搬运失败：error 有值而 state 还停在 WAIT */
    if ((info->state == OTA_INFO_STATE_WAIT) && (info->error != OTA_INFO_ERROR_NONE))
    {
        info->state = OTA_INFO_STATE_IDLE;
        ota_info_update(info);

        ota_send_result(OTA_RESULT_CSUM_FAIL);
        return 1;
    }
    return 0;
}

/* 写 ota_info 并跳 boot，由 uart_protocol_ota_poll 在应答发出后调用 */
void uart_ota_jump_to_boot(void)
{
    ota_info_t *info = ota_info_get(0);

    info->error = OTA_INFO_ERROR_NONE;
    info->state = OTA_INFO_STATE_WAIT;
    ota_info_update(info);
    WDT_CLOSE();

    NVIC_DisableIRQ(SysTick_IRQn);
    IRQn_Type irq = I2C1_IRQn;
    for (irq = I2C1_IRQn; irq <= PWMTM_IRQn; irq++)
        NVIC_DisableIRQ(irq);

    go_boot();
}
