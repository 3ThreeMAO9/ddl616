/**
 * Copyright (c) 2026 GZ-OB, All rights reserved.
 * File name: uart_protocol_linked_unlock.c
 * Desc: 联动开锁 4 位配对码：0x6A 创建 / 0x6B 加入 → 0xEA/0xEB 应答；
 *       退出联动开锁：0x6D → 0xED 应答；
 *       配对/退出结果统一用 0x6C（role+result）上报，门锁回 0xEC；
 *       背板下发联动开锁执行 0x6E → 门锁回 0xEE 并执行开锁
 * Version 1.2.0
 * Date: 2026-11-06
 *
 * A 锁创建、B 锁加入、任意锁退出用同一套代码，仅由菜单传入的 role 区分：
 *   role = LINKED_UNLOCK_ROLE_CREATE(0) → 发 0x6A，等 0xEA，结果 role=0
 *   role = LINKED_UNLOCK_ROLE_JOIN(1)   → 发 0x6B，等 0xEB，结果 role=1
 *   role = LINKED_UNLOCK_ROLE_EXIT(2)   → 发 0x6D，等 0xED，结果 role=2
 */

#include "uart_protocol.h"
#include "msg_protocol.h"
#include "system_timer.h"
#include "event.h"

#define OB_LOG_LEVEL OB_LOG_LEVEL_DEFAULT
#include "ob_log.h"
#define TAG "up_linked_unlock"

#define LINKED_ACK_TIME_OUT     (1000)      // 等 0xEA/0xEB 的超时 ms
#define LINKED_ROLE_NONE        (0xFF)      // 当前没有等待中的联动流程

static uint8_t  s_role = LINKED_ROLE_NONE;  // 当前等待的角色
static uint8_t  s_ack_waiting;              // 1 = 还在等背板 ACK
static uint32_t s_ack_deadline;

static uint16_t s_open_sn;                  // 最近一次已执行的 0x6E 帧序号
static uint8_t  s_open_sn_valid;            // s_open_sn 是否有效

static void linked_unlock_result_push(uint8_t success)
{
    s_role         = LINKED_ROLE_NONE;
    s_ack_waiting  = 0;
    baseEventPush(Q_HANDLE_SIG,
                  success ? EVENT_RESULT_LINKED_UNLOCK_SUCCESS : EVENT_RESULT_LINKED_UNLOCK_FAIL);
}

// 发起创建/加入/退出：pin4 仅创建/加入需要；退出时 role=EXIT，pin4 传 NULL
void uart_linked_unlock_start(uint8_t role, const uint8_t *pin4)
{
    if ((LINKED_UNLOCK_ROLE_CREATE == role || LINKED_UNLOCK_ROLE_JOIN == role) && (NULL == pin4)) {
        return;
    }

    s_role        = role;
    s_ack_waiting = 1;
    s_ack_deadline = system_inc_time_cnt(LINKED_ACK_TIME_OUT);

    switch (role) {
        case LINKED_UNLOCK_ROLE_CREATE:
            uart_msg_linked_unlock_create(pin4);
            break;
        case LINKED_UNLOCK_ROLE_JOIN:
            uart_msg_linked_unlock_join(pin4);
            break;
        case LINKED_UNLOCK_ROLE_EXIT:
            uart_msg_linked_unlock_exit();
            break;
        default:
            s_role = LINKED_ROLE_NONE;
            break;
    }
}

// 退出菜单/超时：丢弃后续应答（迟到的 0x6C 不再触发状态切换）
void uart_linked_unlock_cancel(void)
{
    s_role        = LINKED_ROLE_NONE;
    s_ack_waiting = 0;
}

// 是否还在等背板应答/配对结果（供菜单区分"等待期"与普通停留超时）
uint8_t uart_linked_unlock_is_waiting(void)
{
    return (LINKED_ROLE_NONE != s_role) ? 1 : 0;
}

// 背板 ACK 超时按失败处理
void uart_linked_unlock_poll(void)
{
    if (!s_ack_waiting || !system_out_time_cnt(s_ack_deadline)) {
        return;
    }

    OB_LOGW(TAG, "ack timeout, role=%u", s_role);
    linked_unlock_result_push(0);
}

static void linked_unlock_ack_handle(uint8_t role, uart_packet_t *packet)
{
    if ((LINKED_ROLE_NONE == s_role) || (role != s_role)) {
        return;                                     // 不在流程中 / 角色不符，忽略迟到应答
    }

    s_ack_waiting = 0;

    // ACK 带 1 字节 status：0 = 接收正常，其它视为失败
    if ((packet->length < 1) || (STATUS_SUCCESS != packet->payload[0])) {
        OB_LOGW(TAG, "ack status fail, role=%u", role);
        linked_unlock_result_push(0);
        return;
    }

    OB_LOGI(TAG, "ack ok, role=%u, waiting result", role);
}

// 0xEA：创建指令应答
HANDLER_DEFINE(UP_CMD_LINKED_UNLOCK_CREATE_ACK)
{
    linked_unlock_ack_handle(LINKED_UNLOCK_ROLE_CREATE, packet);
    return 0;
}

// 0xEB：加入指令应答
HANDLER_DEFINE(UP_CMD_LINKED_UNLOCK_JOIN_ACK)
{
    linked_unlock_ack_handle(LINKED_UNLOCK_ROLE_JOIN, packet);
    return 0;
}

// 0xED：退出指令应答
HANDLER_DEFINE(UP_CMD_LINKED_UNLOCK_EXIT_ACK)
{
    linked_unlock_ack_handle(LINKED_UNLOCK_ROLE_EXIT, packet);
    return 0;
}

// 0x6C：配对结果上报（背板 → 门锁）
HANDLER_DEFINE(UP_CMD_LINKED_UNLOCK_RESULT)
{
    uint8_t role;

    uart_msg_empty_ack(packet->cmd);                // 先应答，避免背板重发

    if (packet->length < 2) {
        return 0;
    }

    role = packet->payload[0];

    if ((LINKED_ROLE_NONE == s_role) || (role != s_role)) {
        return 0;                                   // 迟到包 / 角色不符，只应答不动作
    }

    OB_LOGI(TAG, "result role=%u, result=%u", role, packet->payload[1]);

    linked_unlock_result_push((LINKED_UNLOCK_RESULT_SUCCESS == packet->payload[1]) ? 1 : 0);

    return 0;
}

// 0x6E：背板下发联动开锁执行（背板 → 门锁）
HANDLER_DEFINE(UP_CMD_LINKED_UNLOCK_OPEN)
{
    // 先应答：背板 300ms 收不到 0xEE 会重发，应答后停止重传
    uart_msg_empty_ack(packet->cmd);

    // 重发帧 sn 不变：只执行一次开锁，避免重复开锁/重复记录
    if (s_open_sn_valid && (s_open_sn == packet->sn)) {
        OB_LOGI(TAG, "open duplicate, sn=%u ignored", packet->sn);
        return 0;
    }
    s_open_sn       = packet->sn;
    s_open_sn_valid = 1;

    OB_LOGI(TAG, "linked open, sn=%u", packet->sn);
    baseEventPush(Q_HANDLE_SIG, EVENT_RESULT_LINKED_UNLOCK_OPEN);

    return 0;
}
