/**
 * Copyright (c) 2026 GZ-OB, All rights reserved.
 * File name: uart_protocol_prop_front_panel_version.c
 * Desc: 前面板版本（WiFi 模块 <-> 门锁），命令字 0x20，应答 0xA0
 * Version 1.0.0
 * Date: 2026-10-21
 *
 * 只读属性，物模型 string / maxLength 16。
 * payload = "V0.03\0"（6 字节）：PATCH_VERSION 两位补零 + 结尾 '\0'
 * 带 '\0' 是因为 WiFi 侧用 strlen(value) 取长度，其接收缓冲区不保证以 0 结尾。
 * 门锁会在入网成功（0x65=成功）时主动上报一次，见 uart_msg_report_front_panel_version()。
 */

#include "uart_protocol.h"
#include "item_config.h"            // readDeviceVersion()（PATCH_VERSION）

#define OB_LOG_LEVEL OB_LOG_LEVEL_NONE
#include "ob_log.h"
#define TAG "up_prop_fpanel"

#define F_PANEL_VER_STR_LEN     (5)     // "V0.03"
#define F_PANEL_VER_BUF_LEN     (F_PANEL_VER_STR_LEN + 1)   // 含结尾 '\0'

HANDLER_DEFINE(UP_CMD_PROP_FRONT_PANEL_VERSION)
{
    uint16_t version = readDeviceVersion();
    char str[F_PANEL_VER_BUF_LEN];

    str[0] = 'V';
    str[1] = '0';
    str[2] = '.';
    str[3] = (char)('0' + (version / 10) % 10);
    str[4] = (char)('0' + version % 10);
    str[5] = '\0';                          // 供对端 strlen 取长度

    uart_msg_property_ack(packet->cmd, (uint8_t *)str, sizeof(str));

    return 0;
}
