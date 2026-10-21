/**
 * Copyright (c) 2026 GZ-OB, All rights reserved.
 * File name: uart_protocol_totp.c
 * Desc: 一次性密码校验（0x69 请求 / 0xE9 应答）
 * Version 1.0.0
 * Date: 2026-10-21
 */

#include "uart_protocol.h"
#include "msg_protocol.h"
#include "lock_log.h"
#include "tm_data.h"
#include "event.h"
#include "system_timer.h"

#define OB_LOG_LEVEL OB_LOG_LEVEL_NONE
#include "ob_log.h"
#define TAG "up_totp"

#define TOTP_TIMEOUT        (1000)      // 应答超时 ms

static uint8_t  s_totp_waiting;
static uint32_t s_totp_deadline;

// 发起校验（pwd6：6 字节 0~9）
void uart_totp_verify_start(const uint8_t *pwd6)
{
    s_totp_waiting  = 1;
    s_totp_deadline = system_inc_time_cnt(TOTP_TIMEOUT);
    uart_msg_totp_verify(pwd6);
}

// 超时按失败处理
void uart_totp_poll(void)
{
    if (!s_totp_waiting || !system_out_time_cnt(s_totp_deadline)) {
        return;
    }

    s_totp_waiting = 0;
    userHandleEventPush(EVENT_RESULT_FAIL_INVALID, 0);
}

// 0xE9：0=通过
HANDLER_DEFINE(UP_CMD_TOTP_VERIFY_ACK)
{
    if (!s_totp_waiting) {
        return 0;                       // 迟到的应答丢弃
    }

    s_totp_waiting = 0;

    if ((packet->length >= 1) && (0 == packet->payload[0])) {
        lock_log_add_record(KIOT_TM_P_RECORD_EVENT_TYPE_CAO_ZUO_JI_LU,
                            KIOT_TM_P_RECORD_OPERATION_TYPE_KAI_SUO_JI_LU,
                            KIOT_TM_P_RECORD_UNLOCK_TYPE_YI_CI_XING_MI_MA_KAI_SUO, 0xFF);
        userHandleEventPushType(EVENT_RESULT_SUCCESS_VERIFY_USER, 0xFF, VERIFY_SRC_ONE_TIME);
    }
    else {
        userHandleEventPush(EVENT_RESULT_FAIL_INVALID, 0);
    }

    return 0;
}
