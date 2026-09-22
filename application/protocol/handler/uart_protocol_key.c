/**
 * Copyright (c) 2026 GZ-OB, All rights reserved.
 * File name: uart_protocol_key.c
 * Desc:
 * Version: 1.0.0
 * Revision: James_Zhang
 * Date: 2026-09-20
 */

#include "uart_protocol.h"


#define OB_LOG_LEVEL OB_LOG_LEVEL_DEFAULT
#include "ob_log.h"
#define TAG "uart_protocol_key"

static void on_reset_key(uint8_t report)
{
    switch (report)
    {
    case EVENT_KEY_REPORT_DOUBLE:
        OB_LOGI(TAG, "[RESET] DOUBLE 进入恢复出厂设置");
        baseEventPush(Q_HANDLE_SIG, EVENT_RESULT_RESET);
        break;
    default:
        break;
    }
}

static void on_setting_key(uint8_t report)
{
    switch (report)
    {
    case EVENT_KEY_REPORT_SINGLE:
        OB_LOGI(TAG, "[SETTING] SINGLE 添加管理用戶");
        baseEventPush(Q_HANDLE_SIG, EVENT_RESULT_ADD_ADMIN);
        break;
    case EVENT_KEY_REPORT_DOUBLE:
        OB_LOGI(TAG, "[RESET] DOUBLE 进入本地菜单");
        baseEventPush(Q_HANDLE_SIG, EVENT_RESULT_ENTER_LOCAL_MENU);
        break;
    case EVENT_KEY_REPORT_LONG:
        OB_LOGI(TAG, "[RESET] LONG  进入配网状态");
        baseEventPush(Q_HANDLE_SIG, EVENT_RESULT_ENTER_NET_CONFIG);
        break;
    }
}

HANDLER_DEFINE(UP_CMD_KEY)
{
    frame_key_t *key = (frame_key_t *)packet->payload;
    OB_LOGI(TAG, "event_key_id[%u], event_key_report[%u]", key->event_key_id, key->event_key_report);

    switch (key->event_key_id)
    {
    case EVENT_KEY_RESET_ID:
        on_reset_key(key->event_key_report);
        break;
    case EVENT_KEY_SETTING_ID:
        on_setting_key(key->event_key_report);
        break;
    default:
        OB_LOGW(TAG, "   unknown id=%u", key->event_key_id);
        break;
    }
    uart_msg_ack(UP_CMD_KEY_ACK, STATUS_SUCCESS);

    return 0;
}
