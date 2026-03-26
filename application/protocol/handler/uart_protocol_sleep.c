/**
 * Copyright (c) 2025 GZ-OB, All rights reserved.
 * File name: uart_protocol_sleep.c
 * Desc:
 * Version: 1.0.0
 * Revision: James_Zhang
 * Date: 2026-03-11
 */

#include "uart_protocol.h"
#include "task_system_time.h"
#include "event.h"

#define OB_LOG_LEVEL OB_LOG_LEVEL_DEFAULT
#include "ob_log.h"
#define TAG "up_sleep"

HANDLER_DEFINE(UP_CMD_SLEEP)
{
    frame_sleep_t *param = (frame_sleep_t*)(packet->payload);
    uint8_t status = STATUS_FAILED;

    OB_LOGD(TAG, "cmd[%02X] tsn[%02X]", packet->cmd, packet->TSN);
    OB_LOGD(TAG, "status[%02X]", param->status);
    if (param->status == 0xff){

        baseEventPush(Q_HANDLE_SIG, HANDLE_EVENT_SLEEP);
        status = STATUS_SUCCESS;
    }
    uart_msg_ack_sleep(status);
    return 0;
}
