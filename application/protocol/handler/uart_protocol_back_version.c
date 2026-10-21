/**
 * Copyright (c) 2026 GZ-OB, All rights reserved.
 * File name: uart_protocol_back_version.c
 * Desc: 后板(WiFi)版本：0xE8 应答、0x21 读请求
 * Version 1.0.0
 * Date: 2026-10-21
 */

#include "uart_protocol.h"
#include "msg_protocol.h"

#define OB_LOG_LEVEL OB_LOG_LEVEL_NONE
#include "ob_log.h"
#define TAG "up_back_ver"

#define BACK_VER_SEG_CNT    (4)
#define BACK_VER_STR_MAX    (16)

// 收到 WiFi 的版本 → 缓存并上报
HANDLER_DEFINE(UP_CMD_GET_BACK_VERSION_ACK)
{
    if (packet->length >= BACK_VER_SEG_CNT) {
        uart_msg_set_back_version(packet->payload);
        uart_msg_report_back_panel_version();
    }

    return 0;
}

// 读请求 → 回缓存（未取到为 "V0.0.0.0"）
HANDLER_DEFINE(UP_CMD_PROP_BACK_PANEL_VERSION)
{
    char str[BACK_VER_STR_MAX];
    uint16_t len = uart_msg_back_version_str(str, sizeof(str));

    if (0 == len) {
        return 0;
    }

    uart_msg_property_ack(packet->cmd, (uint8_t *)str, (uint16_t)(len + 1));

    return 0;
}
