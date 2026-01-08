/**
 * Copyright (c) 2025 GZ-OB, All rights reserved.
 * File name: uart_protocol_report_param.c
 * Desc:
 * Version: 1.0.0
 * Revision: James_Zhang
 * Date: 2026-01-08
 */

#include "uart_protocol.h"

#define OB_LOG_LEVEL OB_LOG_LEVEL_DEFAULT
#include "ob_log.h"
#define TAG "uart_protocol_report_param"

HANDLER_DEFINE(UP_CMD_REPORT_PARAM)
{
    OB_LOGD(TAG, "cmd[%02X] tsn[%02X]", packet->cmd, packet->TSN);
    frame_report_param_t *param = (frame_report_param_t *)(packet->payload);
    packet->length = BIG_LITTLE_SWAP16(packet->length); // 大小端转换

    if (packet->length % sizeof(frame_work_mode_t) != 0) {
        OB_LOGE(TAG, "PARAM: len(%d) not match group size", packet->length);
        return 0xff;
    }
    uart_msg_ack_param_report(STATUS_SUCCESS);

    OB_LOGD(TAG, "PARAM: len=%d", packet->length);
    uint8_t group_cnt = packet->length / sizeof(frame_work_mode_t);
    // 循环解析每组参数
    for(uint8_t i = 0; i < group_cnt; i++) {
        OB_LOGD(TAG, "PARAM[%d]: src=0x%02X, data=0x%02X",
                i + 1, param[i].event_param, param[i].data);
        switch (param[i].event_param){
        case EVENT_PARAM_TAMPER_ALARM:
            break;
        case EVENT_PARAM_LOITER_ALARM:
            break;
        case EVENT_PARAM_FACE_FUNC_SETTING:
            break;
        case EVENT_PARAM_HUMAN_SENSOR_SETTING:
            break;
        default:
            OB_LOGE(TAG, "[%s] not default", __func__);
            break;
        }
    }

    return 0;
}


