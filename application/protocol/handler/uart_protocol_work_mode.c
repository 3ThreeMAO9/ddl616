/**
 * Copyright (c) 2025 GZ-OB, All rights reserved.
 * File name: uart_protocol_work_mode.c
 * Desc:
 * Version: 1.0.0
 * Revision: James_Zhang
 * Date: 2025-12-02
 */

#include "uart_protocol.h"
#include "task_fingerprint.h"
#include "task_face.h"

#define OB_LOG_LEVEL OB_LOG_LEVEL_DEFAULT
#include "ob_log.h"
#define TAG "up_work_mode"

static void finger_mode_handler(frame_work_mode_t* param)
{
    if (param == NULL) {
        OB_LOGE(TAG, "param is NULL");
        return;
    }

    switch (param->event_code) {
        case FINGER_WORK_MODE_VERIFY:
            OB_LOGD(TAG, "finger verify mode");
            fp_task_set_mode(FP_MODE_VERIFY);
            break;
        case FINGER_WORK_MODE_ADD_USER:
            OB_LOGD(TAG, "finger add user mode");
            fp_task_set_mode(FP_MODE_REGISTER);
            break;
        case FINGER_WORK_MODE_ID_DELETE:
            {
            const fp_delete_params_t delete_params = {
                .page_id = param->data,
                .count = 1,
            };
            OB_LOGD(TAG, "finger ID delete mode ,data=0x%04X",delete_params.page_id);
            fp_task_delete_fp(delete_params);
            }
            break;
        case FINGER_WORK_MODE_VERIFY_DELETE:
            OB_LOGD(TAG, "finger verify delete mode");
            fp_task_set_mode(FP_MODE_VERIFY_DELETE);
            break;
        case FINGER_WORK_MODE_IDLE:
            OB_LOGD(TAG, "finger idle mode");
            fp_task_set_mode(FP_MODE_IDLE);
            break;
        case FINGER_WORK_MODE_SLEEP:
            OB_LOGD(TAG, "finger sleep mode");
            fp_task_set_mode(FP_MODE_SLEEP);
            break;
        case FINGER_WORK_MODE_NULL:
            OB_LOGD(TAG, "finger idle/null mode, ignore");
            fp_task_set_mode(FP_MODE_IDLE);
            break;
        default:
            OB_LOGE(TAG, "unknown finger event code: %d", param->event_code);
            break;
    }
}

static void card_mode_handler(frame_work_mode_t* param)
{
    if (param == NULL) {
        OB_LOGE(TAG, "param is NULL");
        return;
    }
    switch (param->event_code)
    {
    case CARD_WORK_MODE_NULL:
        OB_LOGD(TAG, "card idle/null mode, ignore");
        break;
    case CARD_WORK_MODE_IDLE:
        OB_LOGD(TAG, "card idle mode");
        break;
    case CARD_WORK_MODE_VERIFY:
        OB_LOGD(TAG, "card verify mode");
        break;
    case CARD_WORK_MODE_ADD_USER:
        OB_LOGD(TAG, "card add user mode");
        break;
    case CARD_WORK_MODE_ID_DELETE:
        OB_LOGD(TAG, "card ID delete mode ,del_id = 0x%04X",param->data);
        break;
    case CARD_WORK_MODE_VERIFY_DELETE:
        OB_LOGD(TAG, "card verify delete mode");
        break;
    case CARD_WORK_MODE_READ_SECTOR:
        OB_LOGD(TAG, "card read sector mode");
        break;
    default:
        OB_LOGE(TAG, "unknown card event code: %d", param->event_code);
        break;
    }
}

static void face_mode_handler(frame_work_mode_t* param)
{
    if (param == NULL) {
        OB_LOGE(TAG, "param is NULL");
        return;
    }
    switch (param->event_code)
    {
    case FACE_WORK_MODE_VERIFY:
        OB_LOGD(TAG, "face verify mode");
        if (param->data == VERIFY_SUB_MODE_DEMO)
            face_task_set_mode(FACE_MODE_VERIFY_DEMO);
        else if(param->data == VERIFY_SUB_MODE_NORMAL)
            face_task_set_mode(FACE_MODE_VERIFY);
        break;
    case FACE_WORK_MODE_ADD_USER:
        face_function_attr_t face_attr;
        
        // 步骤1：先初始化结构体默认值
        face_attr.register_count = 5;
        face_attr.repeat = 0;                // 0：查重	1：不查重
        face_attr.register_time_out = 0x0A;  // 录入超时默认值
        face_attr.register_type = 0;         // 0：多帧录入	1：单帧录入

        // 步骤2：从param->data解析位域，赋值给结构体（现在可修改）
        // bit0~bit3：录入超时时间（低4位）
        face_attr.register_time_out = (param->data & 0x0F);
        // bit4：查重开关（右移4位后取最低位）
        face_attr.repeat = ((param->data >> 4) & 0x01);
        // bit5：录入类型（右移5位后取最低位）
        face_attr.register_type = ((param->data >> 5) & 0x01);


        OB_LOGD(TAG, "face_attr.register_time_out[%02X]", face_attr.register_time_out);
        OB_LOGD(TAG, "face_attr.repeat           [%02X]", face_attr.repeat);
        OB_LOGD(TAG, "face_attr.register_type    [%02X]", face_attr.register_type);
        // 设置人脸任务属性
        face_task_set_attr((void*)(&face_attr), sizeof(face_function_attr_t));

        OB_LOGD(TAG, "face add user mode");
        face_task_set_mode(FACE_MODE_REGISTER);
        break;
    case FACE_WORK_MODE_ID_DELETE:
        OB_LOGD(TAG, "face ID delete mode ,del_id = 0x%04X",param->data);
        face_delete_params_t del_id;
        del_id.page_id = param->data;
        face_task_delete_face(del_id);
        break;
    case FACE_WORK_MODE_VERIFY_DELETE:
        OB_LOGD(TAG, "face verify delete mode");
        face_task_set_mode(FACE_MODE_VERIFY_DELETE);
        break;
    case FACE_WORK_MODE_IDLE:
        OB_LOGD(TAG, "face idle mode");
        face_task_set_mode(FACE_MODE_IDLE);
        break;
    case FACE_WORK_MODE_SLEEP:
        OB_LOGD(TAG, "face sleep mode");
        face_task_set_mode(FACE_MODE_SLEEP);
        break;
    case FACE_WORK_MODE_NULL:
        OB_LOGD(TAG, "face idle/null mode, ignore");
        break;
    case FACE_WORK_MODE_ADD_USER_PALM:
        face_function_attr_t palm_attr;
        
        // 步骤1：先初始化结构体默认值
        palm_attr.register_count = 1;
        palm_attr.repeat = 0;                // 0：查重	1：不查重
        palm_attr.register_time_out = 0x0A;  // 录入超时默认值
        palm_attr.register_type = 0;         // 0：多帧录入	1：单帧录入

        // 步骤2：从param->data解析位域，赋值给结构体（现在可修改）
        // bit0~bit3：录入超时时间（低4位）
        palm_attr.register_time_out = (param->data & 0x0F);
        // bit4：查重开关（右移4位后取最低位）
        palm_attr.repeat = ((param->data >> 4) & 0x01);
        // bit5：录入类型（右移5位后取最低位）
        palm_attr.register_type = ((param->data >> 5) & 0x01);

        palm_attr.register_type = 0x03;

        OB_LOGD(TAG, "palm_attr.register_time_out[%02X]", palm_attr.register_time_out);
        OB_LOGD(TAG, "palm_attr.repeat           [%02X]", palm_attr.repeat);
        OB_LOGD(TAG, "palm_attr.register_type    [%02X]", palm_attr.register_type);
        // 设置人脸任务属性
        face_task_set_attr((void*)(&palm_attr), sizeof(face_function_attr_t));

        OB_LOGD(TAG, "palm add user mode");
        face_task_set_mode(FACE_MODE_REGISTER_PALM);
        break;
    default:
        OB_LOGE(TAG, "unknown face event code: %d", param->event_code);
        break;
    }
}

static void tamper_mode_handler(frame_work_mode_t* param)
{
    if (param == NULL) {
        OB_LOGE(TAG, "param is NULL");
        return;
    }
    switch (param->event_code) {
    case TAMPER_WORK_MODE_NULL:
        OB_LOGD(TAG, "tamper idle/null mode, ignore");
        break;
    case TAMPER_WORK_MODE_DISABLE:
        OB_LOGD(TAG, "tamper disable mode");
        break;
    case TAMPER_WORK_MODE_ENABLE:
        OB_LOGD(TAG, "tamper enable mode");
        break;
    default:
        OB_LOGE(TAG, "unknown tamper event code: %d", param->event_code);
        break;
    }
}

HANDLER_DEFINE(UP_CMD_WORK_MODE)
{
    frame_work_mode_t *param = (frame_work_mode_t *)(packet->payload);
    uartTaskRetryClean((packet->cmd & (~0x80)),packet->TSN);    //清空UartTx的重发数据

    packet->length = BIG_LITTLE_SWAP16(packet->length); // 大小端转换

    if (packet->length % sizeof(frame_work_mode_t) != 0) {
        OB_LOGE(TAG, "WORK_MODE: len(%d) not match group size", packet->length);
        return 0xff;
    }
    uart_msg_ack_word_mode(STATUS_SUCCESS);

    OB_LOGD(TAG, "WORK_MODE: len=%d", packet->length);
    uint8_t group_cnt = packet->length / sizeof(frame_work_mode_t);
    // 循环解析每组参数
    for(uint8_t i = 0; i < group_cnt; i++) {
        param[i].data = BIG_LITTLE_SWAP16(param[i].data); // 大小端转换
        // 打印当前组参数（极简日志，包含组号）
        OB_LOGD(TAG, "WORK_MODE[%d]: src=0x%02X, code=0x%02X, data=0x%04X",
                i + 1, param[i].event_source, param[i].event_code, param[i].data);
        switch (param[i].event_source){
        case EVENT_SOURCE_FINGER:
            finger_mode_handler(&param[i]);
            break;
        case EVENT_SOURCE_CARD:
            card_mode_handler(&param[i]);
            break;
        case EVENT_SOURCE_FACE:
            face_mode_handler(&param[i]);
            break;
        case EVENT_SOURCE_TAMPER:
            tamper_mode_handler(&param[i]);
            break;
        default:
            OB_LOGE(TAG, "[%s] not default", __func__);
            break;
        }
    }

    return 0;
}
