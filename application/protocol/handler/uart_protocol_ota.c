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
#include "sfud.h"
#include "hal_wdt.h"
#include <string.h>

#define OB_LOG_LEVEL OB_LOG_LEVEL_DEFAULT
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
    sfud_flash *flash = ota_flash();
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

/* 0x0C 结果上报：[result 1B][version 3B] */
static void ota_send_result(uint8_t result)
{
    uint8_t buf[4] = { result, s_ota.version[0], s_ota.version[1], s_ota.version[2] };
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

    if ((size < 4) || (size > OTA_FLASH_MAX_SIZE))
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

    /* 擦除 64K，耗时 1~2 秒，期间无应答属正常 */
    if (sfud_erase(flash, ota_area_addr(), OTA_FLASH_MAX_SIZE) != SFUD_SUCCESS)
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
            ota_send_result(OTA_RESULT_SUCCESS);
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

        if (sfud_write(flash, ota_area_addr() + s_ota.expected, wr,
                p + 4 + skip) != SFUD_SUCCESS)
        {
            ret = OTA_DATA_STATUS_WRITE_FAIL;
        }
        else
        {
            s_ota.calc = ota_checksum_add(s_ota.calc, p + 4 + skip, wr);
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
