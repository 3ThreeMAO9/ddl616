/**
 * Copyright (c) 2026 GZ-OB, All rights reserved.
 * File name: uart_protocol_key.c
 * Desc:
 * Version: 1.0.0
 * Revision: James_Zhang
 * Date: 2026-09-20
 */

#include "uart_protocol.h"


#define OB_LOG_LEVEL OB_LOG_LEVEL_NONE
#include "ob_log.h"
#define TAG "up_key"

static void on_reset_key(uint8_t report)
{
    switch (report)
    {
    case EVENT_KEY_REPORT_SINGLE:
        baseEventPush(Q_HANDLE_SIG, EVENT_RESULT_BACK_KEY_SINGLE);
        break;
    case EVENT_KEY_REPORT_DOUBLE:
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
        baseEventPush(Q_HANDLE_SIG, EVENT_RESULT_ADD_ADMIN);
        break;
    case EVENT_KEY_REPORT_DOUBLE:
        baseEventPush(Q_HANDLE_SIG, EVENT_RESULT_ENTER_LOCAL_MENU);
        break;
    case EVENT_KEY_REPORT_LONG:
        baseEventPush(Q_HANDLE_SIG, EVENT_RESULT_ENTER_NET_CONFIG);
        break;
    }
}

HANDLER_DEFINE(UP_CMD_KEY)
{
    frame_key_t *key = (frame_key_t *)packet->payload;

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
    uart_msg_ack(packet->cmd, STATUS_SUCCESS);

    return 0;
}
