/**
 * Copyright (c) 2026 GZ-OB, All rights reserved.
 * File name: uart_protocol_prop_lock_status.c
 * Desc: 门锁状态（WiFi 模块 -> 门锁），命令字 0x23 / 应答码 0xA3（PROP_CMD_LOCK_STATUS）
 * Version 1.0.0
 * Date: 2026-10-16
 *
 * 只读。应答 = 最近一次电机动作：1=关锁 2=开锁（对齐 p_lock_status）。
 */

#include "uart_protocol.h"
#include "task_motor.h"

HANDLER_DEFINE(UP_CMD_PROP_LOCK_STATUS)
{
    uint8_t status = motorTaskReadDoorState();

    (void)packet;

    uart_msg_property_send(UP_CMD_PROP_LOCK_STATUS, &status, sizeof(status));

    return 0;
}
