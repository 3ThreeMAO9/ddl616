/**
 * Copyright (c) 2026 GZ-OB, All rights reserved.
 * File name: uart_protocol_wifi_test.c
 * Desc: WiFi RSSI 产测（258# 第五步）
 *       前板发 0x76 让背板测 WiFi RSSI，背板先回 0xF6(status) ACK，
 *       测完主动上报 0x77 + [rssi_dbm 1B 有符号]，RSSI >= -30dBm 视为合格。
 * Version 1.0.0
 * Date: 2026-11-16
 */

#include "uart_protocol.h"
#include "msg_protocol.h"
#include "system_timer.h"
#include "event.h"

#define OB_LOG_LEVEL OB_LOG_LEVEL_DEFAULT
#include "ob_log.h"
#define TAG "up_wifi_test"

/* 阶段 */
#define WIFI_TEST_STATE_IDLE        (0)     // 空闲
#define WIFI_TEST_STATE_WAIT_ACK    (1)     // 已发 0x76，等 0xF6 ACK
#define WIFI_TEST_STATE_WAIT_RSSI   (2)     // 收到 0xF6，等 0x77 RSSI 上报

/* 超时（ms） */
#define WIFI_TEST_ACK_TIME_OUT      (2000)  // 等 0xF6 ACK
#define WIFI_TEST_RSSI_TIME_OUT     (15000) // 等 0x77 RSSI（背板测 WiFi 可能要几秒钟）

static uint8_t  s_state;
static uint32_t s_deadline;

static void wifi_test_push_result(uint8_t pass)
{
    s_state = WIFI_TEST_STATE_IDLE;
    uart_self_test_result_set(SELF_TEST_ITEM_WIFI, pass ? 1 : 0);
    userHandleEventPush(pass ? EVENT_RESULT_WIFI_TEST_SUCCESS : EVENT_RESULT_WIFI_TEST_FAIL, 0);
}

// 发起 WiFi 测试：258# 第四步三个实体按键全通过后调用
void uart_wifi_test_start(void)
{
    s_state    = WIFI_TEST_STATE_WAIT_ACK;
    s_deadline = system_inc_time_cnt(WIFI_TEST_ACK_TIME_OUT);

    OB_LOGI(TAG, "start wifi test, send 0x76");
    uart_msg_wifi_test_start();                 // 0x76
}

// 状态机退出或测试失败，丢弃后续迟到应答
void uart_wifi_test_cancel(void)
{
    s_state = WIFI_TEST_STATE_IDLE;
}

uint8_t uart_wifi_test_is_waiting(void)
{
    return (WIFI_TEST_STATE_IDLE != s_state) ? 1 : 0;
}

void uart_wifi_test_poll(void)
{
    if (!uart_wifi_test_is_waiting() || !system_out_time_cnt(s_deadline)) {
        return;
    }

    OB_LOGW(TAG, "timeout, state=%u", s_state);
    s_state = WIFI_TEST_STATE_IDLE;
    wifi_test_push_result(0);
}

// 0xF6：背板对 0x76 的 ACK（status=0 表示开始测试）
HANDLER_DEFINE(UP_CMD_WIFI_TEST_START_ACK)
{
    OB_LOGI(TAG, "recv 0xF6, len=%u, state=%u", packet->length, s_state);

    if (WIFI_TEST_STATE_WAIT_ACK != s_state) {
        OB_LOGW(TAG, "ignore 0xF6: not waiting ack (state=%u)", s_state);
        return 0;
    }

    if ((packet->length < 1) || (STATUS_SUCCESS != packet->payload[0])) {
        OB_LOGW(TAG, "ack status fail: len=%u, status=%u",
                packet->length, packet->length >= 1 ? packet->payload[0] : 0xFF);
        s_state = WIFI_TEST_STATE_IDLE;
        wifi_test_push_result(0);
        return 0;
    }

    OB_LOGI(TAG, "ack ok, waiting rssi");
    s_state    = WIFI_TEST_STATE_WAIT_RSSI;
    s_deadline = system_inc_time_cnt(WIFI_TEST_RSSI_TIME_OUT);

    return 0;
}

// 0x77：背板上报 RSSI 结果
HANDLER_DEFINE(UP_CMD_WIFI_TEST_RSSI)
{
    OB_LOGI(TAG, "recv 0x77, len=%u, state=%u", packet->length, s_state);

    uart_msg_empty_ack(packet->cmd);                // 先 ACK，避免背板重发

    if (WIFI_TEST_STATE_WAIT_RSSI != s_state) {
        OB_LOGW(TAG, "ignore 0x77: not waiting rssi (state=%u)", s_state);
        return 0;
    }

    if (packet->length < 1) {
        OB_LOGW(TAG, "rssi too short (len=%u)", packet->length);
        s_state = WIFI_TEST_STATE_IDLE;
        wifi_test_push_result(0);
        return 0;
    }

    {
        int8_t rssi = (int8_t)packet->payload[0];  // 有符号解析
        uint8_t pass = (rssi >= WIFI_TEST_RSSI_PASS_DBM) ? 1 : 0;

        OB_LOGI(TAG, "rssi=%d dBm, %s", rssi, pass ? "PASS" : "FAIL");

        s_state = WIFI_TEST_STATE_IDLE;
        wifi_test_push_result(pass);
    }

    return 0;
}
