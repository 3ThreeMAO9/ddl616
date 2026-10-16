/**
 * Copyright (c) 2026 GZ-OB, All rights reserved.
 * File name: uart_protocol_prop_solar_panels.c
 * Desc: 太阳能板连接状态（WiFi 模块 -> 门锁），命令字 0x33 / 应答码 0xB3（PROP_CMD_SOLAR_PANELS_CONN_STATUS）
 * Version 1.0.0
 * Date: 2026-10-16
 *
 * 只读。门锁侧无检测：固定回"已断开"。
 */

#include "uart_protocol.h"
#include "tm_data.h"

HANDLER_DEFINE(UP_CMD_PROP_SOLAR_PANELS_CONN_STATUS)
{
    uint8_t status = KIOT_TM_P_SOLAR_PANELS_CONNECTION_STATUS_TAI_YANG_NENG_BAN_YI_DUAN_KAI;

    (void)packet;

    uart_msg_property_send(UP_CMD_PROP_SOLAR_PANELS_CONN_STATUS, &status, sizeof(status));

    return 0;
}
