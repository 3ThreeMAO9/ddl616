/**
 * Copyright (c) 2025 GZ-OB, All rights reserved.
 * File name: uart_protocol_version.c
 * Desc:
 * Version: 1.0.0
 * Revision: James_Zhang
 * Date: 2026-03-25
 */

#include "uart_protocol.h"
#include "task_system_time.h"
#include "task_key.h"
#include "event.h"
#include "fmc.h"
#include "user_parameter.h"

#define OB_LOG_LEVEL OB_LOG_LEVEL_NONE
#include "ob_log.h"
#define TAG "up_touch_sensitivity"

HANDLER_DEFINE(UP_CMD_TOUCH_SENSITIVITY)
{
    frame_touch_sensitivity_t *param = (frame_touch_sensitivity_t *)(packet->payload);
    uint8_t status = STATUS_FAILED;

    OB_LOGD(TAG, "cmd[%02X] tsn[%02X]", packet->cmd, packet->TSN);
    OB_LOGD(TAG, "event[%02X]", param->event);
    OB_LOGD_DUMP(param->data,sizeof(frame_touch_sensitivity_t));

    // 触摸灵敏度处理
    write_touch_chip_sensitivity(param->data);

    keyTaksParameter(param->data);

    status = STATUS_SUCCESS;
    uart_msg_ack_touch_sensitivity(status);

    return 0;
}
