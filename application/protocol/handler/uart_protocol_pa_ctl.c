/**
 * Copyright (c) 2026 GZ-OB, All rights reserved.
 * File name: uart_protocol_pa_ctl.c
 * Desc:
 * Version: 1.0.0
 * Revision: James_Zhang
 * Date: 2026-05-12
 */

#include "uart_protocol.h"
#include "task_system_time.h"
#include "task_key.h"
#include "event.h"
#include "fmc.h"
#include "user_parameter.h"

#define OB_LOG_LEVEL OB_LOG_LEVEL_NONE
#include "ob_log.h"
#define TAG "up_touch_pa_ctl"

HANDLER_DEFINE(UP_CMD_PA_CTL)
{
    frame_pa_ctl_t *param = (frame_pa_ctl_t *)(packet->payload);
    uint8_t status = STATUS_FAILED;

    OB_LOGD(TAG, "cmd[%02X] tsn[%02X]", packet->cmd, packet->TSN);
    OB_LOGD(TAG, "data[%02X]", param->data);

    if (param->data == 0x01)
        HAL_GPIO_Write(VOICE_EN_GPIO, VOICE_EN_PIN, 1);
    else if (param->data == 0x00)
        HAL_GPIO_Write(VOICE_EN_GPIO, VOICE_EN_PIN, 0);

    status = STATUS_SUCCESS;
    uart_msg_ack_pa_ctl(status);

    return 0;
}
