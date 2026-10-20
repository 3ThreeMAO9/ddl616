/**
 * Copyright (c) 2026 GZ-OB, All rights reserved.
 * File name: uart_protocol_prop_battery_info.c
 * Desc: 电池信息（WiFi 模块 -> 门锁），命令字 0x31 / 应答码 0xB1（PROP_CMD_BATTERY_INFO）
 * Version 1.0.0
 * Date: 2026-10-16
 *
 * 只读。应答 = kiot_tm_prop_p_battery_info_stu_t（37 字节，单电池）。
 * 门锁侧只有电量：充电次数/SN 固定 0、充电状态固定"未充电"。
 */

#include "uart_protocol.h"
#include "task_battery.h"
#include "tm_data.h"

HANDLER_DEFINE(UP_CMD_PROP_BATTERY_INFO)
{
    kiot_tm_prop_p_battery_info_stu_t info;

    (void)packet;

    memset(&info, 0, sizeof(info));

    info.p_battery_index       = 1;                                         // 单电池
    info.p_battery_electricity = batteryTaskReadPercent();
    info.p_charging_state      = KIOT_TM_P_CHARGING_STATE_WEI_CHONG_DIAN;   // 无充电检测

    uart_msg_property_ack(packet->cmd, (uint8_t*)&info, sizeof(info));

    return 0;
}
