/**
 * Copyright (c) 2026 GZ-OB, All rights reserved.
 * File name: uart_protocol_version_report.c
 * Desc: 版本主动上报（0x7A 前板版本 / 0x7B 语音包版本）
 *       上电后门锁主动推给 WiFi 背板，背板缓存后供产测 0x0B devnum=6/8 读取；
 *       1s 内没收到 ACK(0xFA/0xFB) 就重发，直到应答为止。
 * Version 1.0.0
 * Date: 2026-11-17
 */

#include "uart_protocol.h"
#include "msg_protocol.h"
#include "system_timer.h"

#define OB_LOG_LEVEL OB_LOG_LEVEL_DEFAULT
#include "ob_log.h"
#define TAG "up_ver_report"

#define VER_REPORT_FIRST_DELAY_MS   (1000)  // 上电后首次上报延时，等 WiFi 背板起来
#define VER_REPORT_INTERVAL_MS      (1000)  // 未收到 ACK 的重发间隔

static uint8_t  s_started;      // 是否已完成首次上报
static uint8_t  s_front_done;   // 前板版本已收到 ACK
static uint8_t  s_voice_done;   // 语音包版本已收到 ACK
static uint32_t s_next_send;    // 下次发送时间点

static void version_report_send_front(void)
{
    char str[16];
    uint16_t len = uart_msg_front_version_str(str, sizeof(str));

    if (0 == len) {
        OB_LOGW(TAG, "front version not ready");
        return;
    }

    OB_LOGI(TAG, "report front version: %s", str);
    uart_msg_property_report(UP_CMD_REPORT_FRONT_VERSION, (uint8_t *)str, len);
}

static void version_report_send_voice(void)
{
    char str[8];
    uint16_t len = uart_msg_voice_version_str(str, sizeof(str));

    if (0 == len) {
        OB_LOGW(TAG, "voice version not ready");
        return;
    }

    OB_LOGI(TAG, "report voice version: %s", str);
    uart_msg_property_report(UP_CMD_REPORT_VOICE_VERSION, (uint8_t *)str, len);
}

// 未收到应答就一直重发（uart_protocol_poll 里调用）
void uart_version_report_poll(void)
{
    if (!s_started) {
        s_started   = 1;
        s_next_send = system_inc_time_cnt(VER_REPORT_FIRST_DELAY_MS);
        return;
    }

    if ((s_front_done && s_voice_done) || !system_out_time_cnt(s_next_send)) {
        return;
    }

    if (!s_front_done) {
        version_report_send_front();
    }

    if (!s_voice_done) {
        version_report_send_voice();
    }

    s_next_send = system_inc_time_cnt(VER_REPORT_INTERVAL_MS);
}

// 0xFA：背板对 0x7A 的 ACK
HANDLER_DEFINE(UP_CMD_REPORT_FRONT_VERSION_ACK)
{
    if (!s_front_done) {
        s_front_done = 1;
        OB_LOGI(TAG, "front version acked, len=%u", packet->length);
    }

    return 0;
}

// 0xFB：背板对 0x7B 的 ACK
HANDLER_DEFINE(UP_CMD_REPORT_VOICE_VERSION_ACK)
{
    if (!s_voice_done) {
        s_voice_done = 1;
        OB_LOGI(TAG, "voice version acked, len=%u", packet->length);
    }

    return 0;
}
