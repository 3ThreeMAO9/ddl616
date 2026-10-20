/**
 * Copyright (c) 2026 GZ-OB, All rights reserved.
 * File name: uart_protocol_join_net_result.c
 * Desc: 入网(蓝牙绑定)结果，命令字 0x65（WiFi 模块 -> 门锁）
 * Version 1.0.0
 * Date: 2026-10-19
 *
 * WiFi 在蓝牙绑定完成时主动下发：payload = [0]=成功 / [1]=失败。
 * 门锁收到后播报"配网成功/失败"并退出配网模式；应答为通用 ACK 0xE5。
 */

#include "uart_protocol.h"
#include "task_hmi.h"
#include "event.h"

HANDLER_DEFINE(UP_CMD_JOIN_NET_RESULT)
{
    uint8_t result = 0;

    if (packet->length >= 1) {
        result = packet->payload[0];
    }

    uart_msg_empty_ack(packet->cmd);     // 先应答，避免对端重发

    if (!hmi_task_join_net_is_busy()) {
        return 0;                                       // 已不在配网流程（超时退出/重复包），只应答
    }

    hmi_task_join_net_time(0);                          // 退出配网：停蓝灯闪烁、允许休眠

    if (0 == result) {
        hmiTaskSetState(HMI_STATE_JOIN_NET_SUCCESS);    // 绿灯 + "配网成功"
    }
    else {
        hmiTaskSetState(HMI_STATE_JOIN_NET_FAIL);       // 红灯 + "配网失败"
    }

    // 回待机并保持唤醒：WiFi 随后要同步用户表/开锁记录/属性等
    baseEventPush(Q_HANDLE_SIG, EVENT_RESULT_JOIN_NET_DONE);

    return 0;
}
