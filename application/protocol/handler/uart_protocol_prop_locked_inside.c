/**
 * Copyright (c) 2026 GZ-OB, All rights reserved.
 * File name: uart_protocol_prop_locked_inside.c
 * Desc: 门内反锁状态（WiFi 模块 -> 门锁），命令字 0x24 / 应答码 0xA4（PROP_CMD_LOCKED_INSIDE_STATUS）
 * Version 1.0.0
 * Date: 2026-10-16
 *
 * 只读。门锁侧暂无反锁检测：固定回未反锁(0)。
 */

#include "uart_protocol.h"
#include "tm_data.h"

HANDLER_DEFINE(UP_CMD_PROP_LOCKED_INSIDE_STATUS)
{
    uint8_t status = KIOT_TM_P_LOCKED_INSIDE_STATUS_WEI_FAN_SUO;

    (void)packet;

    uart_msg_property_send(UP_CMD_PROP_LOCKED_INSIDE_STATUS, &status, sizeof(status));

    return 0;
}
