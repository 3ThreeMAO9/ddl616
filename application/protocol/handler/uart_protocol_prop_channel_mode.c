/**
 * Copyright (c) 2026 GZ-OB, All rights reserved.
 * File name: uart_protocol_prop_channel_mode.c
 * Desc: 通道(常开)模式（WiFi 模块 -> 门锁），命令字 0x2b / 0x2c / 0x2d
 * Version 1.0.0
 * Date: 2026-10-16
 *
 * 0x2b 开关（读写：0=关闭 1=开启）
 * 0x2c 策略（读写：0=永久 1=自定义）
 * 0x2d 时间计划（读写，最多 4 组；仅策略=自定义时有效）
 * 注：计划的生效判断（按时间开/关锁）待 RTC 时间同步后再做
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

// 0x2c 通道模式策略（0=永久 / 1=自定义）
HANDLER_DEFINE(UP_CMD_PROP_CHANNEL_MODE_POLICY)
{
    uint8_t opt = (packet->length >= 1) ? packet->payload[0] : UP_CMD_PROP_OPT_READ;
    channel_plan_t *cp = getChannelPlan();
    uint8_t policy = cp->policy;

    if ((UP_CMD_PROP_OPT_WRITE == opt) && (packet->length >= 2)
        && (packet->payload[1] <= KIOT_TM_P_CHANNEL_MODE_POLICY_ZI_DING_YI))
    {
        // 只改策略，保留已设的 plan[]：切回"自定义"时计划仍然有效
        cp->policy = packet->payload[1];
        saveChannelPlan();
        policy = cp->policy;
    }

    uart_msg_property_ack(UP_CMD_PROP_CHANNEL_MODE_POLICY_ACK, &policy, sizeof(policy));

    return 0;
}

// 0x2d 通道模式时间计划（最多 CHANNEL_PLAN_MAX 组）
HANDLER_DEFINE(UP_CMD_PROP_CHANNEL_MODE)
{
    uint8_t opt = (packet->length >= 1) ? packet->payload[0] : UP_CMD_PROP_OPT_READ;
    channel_plan_t *cp = getChannelPlan();
    const uint16_t unit = sizeof(kiot_tm_prop_p_channel_mode_stu_t);

    if ((UP_CMD_PROP_OPT_WRITE == opt) && (packet->length > 1))
    {
        uint16_t n = (uint16_t)((packet->length - 1) / unit);

        if (n > CHANNEL_PLAN_MAX)
        {
            n = CHANNEL_PLAN_MAX;
        }

        memcpy(cp->plan, packet->payload + 1, n * unit);
        cp->plan_cnt = (uint8_t)n;
        cp->policy = KIOT_TM_P_CHANNEL_MODE_POLICY_ZI_DING_YI;      // 下发计划即视为自定义
        saveChannelPlan();
    }

    // 永久模式或无计划：回空
    if ((KIOT_TM_P_CHANNEL_MODE_POLICY_YONG_JIU == cp->policy) || (0 == cp->plan_cnt))
    {
        uart_msg_property_ack(UP_CMD_PROP_CHANNEL_MODE_ACK, NULL, 0);
    }
    else
    {
        uart_msg_property_ack(UP_CMD_PROP_CHANNEL_MODE_ACK, (uint8_t *)cp->plan, (uint16_t)(cp->plan_cnt * unit));
    }

    return 0;
}
