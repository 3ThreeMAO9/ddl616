/**
 * Copyright (c) 2026 GZ-OB, All rights reserved.
 * File name: uart_protocol_prop_mode_defense.c
 * Desc: 布防模式（WiFi 模块 -> 门锁），命令字 0x17 / 应答码 0x97（PROP_CMD_MODE_DEFENSE）
 * Version 1.0.0
 * Date: 2026-10-16
 *
 * payload = [opt(1字节), value(1字节)]：opt 0=读取 / 1=设置。
 * 门锁侧暂无布防功能：固定回撤防(0)，写入不落地。
 */

#include "uart_protocol.h"
#include "tm_data.h"

HANDLER_DEFINE(UP_CMD_PROP_MODE_DEFENSE)
{
    uint8_t mode = KIOT_TM_P_MODE_DEFENSE_CHE_FANG;

    (void)packet;

    uart_msg_property_ack(packet->cmd, &mode, sizeof(mode));

    return 0;
}
