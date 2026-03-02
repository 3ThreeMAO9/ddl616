/**
 * Copyright (c) 2025 GZ-OB, All rights reserved.
 * File name: uart_protocol_reset.c
 * Desc:
 * Version: 1.0.0
 * Revision: James_Zhang
 * Date: 2025-12-30
 */

#include "uart_protocol.h"
#include "task_face.h"
#include "task_fingerprint.h"

#define OB_LOG_LEVEL OB_LOG_LEVEL_DEFAULT
#include "ob_log.h"
#define TAG "up_reset"

HANDLER_DEFINE(UP_CMD_RESET)
{
    frame_reset_t *param = (frame_reset_t*)(packet->payload);
    uint8_t status = STATUS_FAILED;

    OB_LOGD(TAG, "cmd[%02X] tsn[%02X]", packet->cmd, packet->TSN);
    OB_LOGD(TAG, "status[%02X]", param->status);
    if (param->status == 0xff){
        // 恢复出厂设置
        const fp_delete_params_t delete_params = {
            .page_id = 0xFFFF,
            .count = 0,
        };
        OB_LOGD(TAG, "finger ID delete mode ,data=0x%04X",delete_params.page_id);
        fp_task_delete_fp(delete_params);


        face_delete_params_t del_id;
        del_id.page_id = 0xFFFF;
        face_task_delete_face(del_id);


        status = STATUS_SUCCESS;
    }
    uart_msg_ack_reset(status);
    return 0;
}
