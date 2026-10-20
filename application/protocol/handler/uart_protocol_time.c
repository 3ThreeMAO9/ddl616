/**
 * Copyright (c) 2026 GZ-OB, All rights reserved.
 * File name: uart_protocol_time.c
 * Desc: 时间同步（《串口时间同步对接说明 v1.0》）
 *       - 0x66 时间下发（WiFi -> 门锁），门锁回 0xE6
 *       - 0xE7 时间查询应答（WiFi -> 门锁）
 * Version 1.2.0
 * Date: 2026-10-20
 *
 * payload（8 字节，小端）：[0..3] UTC 秒  [4..7] 时区偏移（秒，东正西负）
 * 空 payload 的 0xE7 = WiFi 暂无有效时间，门锁保持原状（不要当成 1970）。
 *
 * 查询节奏统一由 uart_protocol_poll() 的心跳驱动（HEART_TIME_OUT = 3s）：
 *   - 串口静默 3s 发一包 0x67（兼作 WiFi 保活）
 *   - 收到任何有效帧都会重置计时
 *   - 休眠唤醒时用 uart_protocol_heart_expire() 让下一圈立刻补一包
 * 发送 0x67 不重置休眠计时；应答 0xE7 属于 ACK，按通用规则也不重置。
 */

#include "uart_protocol.h"
#include "msg_protocol.h"
#include "hal_rtc.h"

#define OB_LOG_LEVEL OB_LOG_LEVEL_NONE
#include "ob_log.h"
#define TAG "up_time"

#define TIME_PAYLOAD_LEN        (8)
#define TIME_TZ_STEP_SEC        (15 * 60)       // hal_rtc 的 time_zone 单位是 15 分钟

#define TIME_MIN_VALID_UTC      (1767225600UL)  // 2026-01-01 00:00:00 UTC，早于它视为无效
#define TIME_TZ_MAX_SEC         (14 * 3600)     // 时区合法范围 ±14 小时

static int32_t time_get_le32(const uint8_t *p)
{
    return (int32_t)((uint32_t)p[0]
                   | ((uint32_t)p[1] << 8)
                   | ((uint32_t)p[2] << 16)
                   | ((uint32_t)p[3] << 24));
}

// 应用时间：只接受"真实可靠"的值（时间戳不早于 2026、时区在 ±14h 内）
static void time_apply(const uint8_t *payload)
{
    int32_t utc = time_get_le32(payload);
    int32_t tz  = time_get_le32(payload + 4);

    if (((uint32_t)utc < TIME_MIN_VALID_UTC)
        || (tz > TIME_TZ_MAX_SEC) || (tz < (-TIME_TZ_MAX_SEC))) {
        OB_LOGW(TAG, "invalid time: utc=%d tz=%d", (int)utc, (int)tz);
        return;
    }

    hal_set_rtc_time((time_t)utc, (int16_t)(tz / TIME_TZ_STEP_SEC));

    OB_LOGW(TAG, "time synced: utc=%d tz=%d", (int)utc, (int)tz);
}

// 0x66 时间下发：更新本地时间并回应答（重传帧内容相同，重复设置无副作用）
HANDLER_DEFINE(UP_CMD_TIME_PUSH)
{
    if (packet->length >= TIME_PAYLOAD_LEN) {
        time_apply(packet->payload);
    }

    uart_msg_empty_ack(packet->cmd);

    return 0;
}

// 0xE7 查询应答：带 8 字节且合法则更新；空包表示对端暂无有效时间，保持原状
HANDLER_DEFINE(UP_CMD_TIME_QUERY_ACK)
{
    if (packet->length >= TIME_PAYLOAD_LEN) {
        time_apply(packet->payload);
    }

    return 0;
}
