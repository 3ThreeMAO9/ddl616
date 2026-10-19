/**
 * Copyright (c) 2026 GZ-OB, All rights reserved.
 * File name: uart_protocol_prop_channel_mode.c
 * Desc: 通道(常开)模式（WiFi 模块 -> 门锁），命令字 0x2b / 0x2c / 0x2d
 * Version 1.0.0
 * Date: 2026-10-16
 *
 * 0x2b 开关（读写：0=关闭 1=开启）
 * 0x2c 策略 / 0x2d 时段：暂不实现（需先做 RTC 时间同步），固定回"永久"/空，
 *       仅用于应答，避免 APP 等待超时
 */

#include "uart_protocol.h"
#include "parameter.h"
#include "task_hmi.h"
#include "task_motor.h"
#include "tm_data.h"

// 0x2b 通道模式开关
HANDLER_DEFINE(UP_CMD_PROP_CHANNEL_MODE_ENABLED)
{
    uint8_t opt = (packet->length >= 1) ? packet->payload[0] : UP_CMD_PROP_OPT_READ;
    uint8_t on = (uint8_t)readUserParameter(USER_PARA_CHANNEL_MODE_ID);

    if ((UP_CMD_PROP_OPT_WRITE == opt) && (packet->length >= 2)
        && (packet->payload[1] <= KIOT_TM_P_CHANNEL_MODE_ENABLED_KAI_QI))
    {
        uint8_t old = on;

        setUserParameter(USER_PARA_CHANNEL_MODE_ID, packet->payload[1]);
        on = (uint8_t)readUserParameter(USER_PARA_CHANNEL_MODE_ID);

        if (on != old)
        {
            // 复用老化流程的开/关锁动作（参数均为 0）
            if (Enabled == on)
            {
                motorTaskHandle(MOTOR_HANDLE_OPEN, 0);      // 开启通道模式：开锁
            }
            else
            {
                motorTaskHandle(MOTOR_HANDLE_CLOSE, 0);     // 关闭通道模式：关锁
            }

            hmiTaskSetState(HMI_STATE_HANDLE_SUCCESS);      // 声光提示：与参数设置成功相同
        }
    }

    uart_msg_property_ack(UP_CMD_PROP_CHANNEL_MODE_ENABLED_ACK, &on, sizeof(on));

    return 0;
}

// 0x2c 策略：固定回"永久"
HANDLER_DEFINE(UP_CMD_PROP_CHANNEL_MODE_POLICY)
{
    uint8_t policy = KIOT_TM_P_CHANNEL_MODE_POLICY_YONG_JIU;

    (void)packet;

    uart_msg_property_ack(UP_CMD_PROP_CHANNEL_MODE_POLICY_ACK, &policy, sizeof(policy));

    return 0;
}

// 0x2d 时段：固定回空
HANDLER_DEFINE(UP_CMD_PROP_CHANNEL_MODE)
{
    (void)packet;

    uart_msg_property_ack(UP_CMD_PROP_CHANNEL_MODE_ACK, NULL, 0);

    return 0;
}
