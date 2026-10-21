/**
 * Copyright (c) 2026 GZ-OB, All rights reserved.
 * File name: uart_protocol_prop_front_panel_version.c
 * Desc: 前面板版本（0x20 / 应答 0xA0）
 * Version 1.1.0
 * Date: 2026-10-21
 */

#include "uart_protocol.h"
#include "msg_protocol.h"

#define OB_LOG_LEVEL OB_LOG_LEVEL_NONE
#include "ob_log.h"
#define TAG "up_prop_fpanel"

#define F_PANEL_VER_BUF_LEN     (16)

HANDLER_DEFINE(UP_CMD_PROP_FRONT_PANEL_VERSION)
{
    char str[F_PANEL_VER_BUF_LEN];
    uint16_t len = uart_msg_front_version_str(str, sizeof(str));

    if (0 == len) {
        return 0;
    }

    uart_msg_property_ack(packet->cmd, (uint8_t *)str, (uint16_t)(len + 1));

    return 0;
}
