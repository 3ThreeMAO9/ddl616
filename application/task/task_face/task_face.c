/**
 * Copyright (c) 2025 GZ-OB, All rights reserved.
 * File name: task_face.c
 * Desc: 人脸识别模块任务层定义
 * Version: 1.0.0
 * Revision: James
 * Date: 2025-12-15
 */
#include "task_face.h"
#include "task_system_time.h"

#include "event.h"
#include "msg_protocol.h"

#define OB_LOG_LEVEL OB_LOG_LEVEL_DEFAULT
#include "ob_log.h"
#define TAG "task_face"

/***************Variable***************/
static face_task_driver_t face_task_driver;

// ------------------------------------------
static void face_init_event_callback(uint8_t result, void* para, uint8_t lenth) {
    OB_LOGI(TAG, "init event: result[%u]", result);
    OB_LOGI_DUMP((uint8_t*)(para), lenth);

    // user add...
    // send face' attr to rear_board (type + count + index_table[])

    face_task_set_mode(FACE_TASK_MODE_DEFAULT); // 上电初始化完成，自动切至默认模式下
}

static void face_verify_event_callback(uint8_t result, void* para, uint8_t lenth) {
    // OB_LOGI(TAG, "verify event: result[%u]", result);
    // OB_LOGI_DUMP((uint8_t*)(para), lenth);

    switch (result)
    {
    case FACE_RESULT_SUCCESS_VERIFY:
        OB_LOGD(TAG, "FACE_RESULT_SUCCESS_VERIFY");
        uart_msg_face(EVENT_CODE_FACE_VERIFY_SUCCESS, (uint8_t *)(para), lenth);
        break;
    case PALM_RESULT_SUCCESS_VERIFY:
        OB_LOGD(TAG, "PALM_RESULT_SUCCESS_VERIFY");
        uart_msg_face(EVENT_CODE_PALM_VERIFY_SUCCESS, (uint8_t *)(para), lenth);
        break;
    case FACE_RESULT_FAIL_UNKNOWNUSER:
        OB_LOGD(TAG, "FACE_RESULT_FAIL_UNKNOWNUSER");
        uart_msg_face(EVENT_CODE_FACE_VERIFY_FAIL, NULL, 0);
        break;
    case FACE_RESULT_FAIL_TIMEOUT:
        OB_LOGD(TAG, "FACE_RESULT_FAIL_TIMEOUT");
        uart_msg_face(EVENT_CODE_FACE_TIME_OUT, NULL, 0);
        break;
    default:
        OB_LOGE(TAG, "[%s] not default", __func__);
        break;
    }

    face_task_set_mode(FACE_MODE_SLEEP);
}

static void face_register_event_callback(uint8_t result, void* para, uint8_t lenth) {
    // OB_LOGI(TAG, "register event: result[%u]", result);
    // OB_LOGI_DUMP((uint8_t*)(para), lenth);
    switch (result)
    {
    case FACE_RESULT_SUCCESS_REGISTER:
        OB_LOGD(TAG, "FACE_RESULT_SUCCESS_REGISTER");
        uart_msg_face(EVENT_CODE_FACE_REGISTER_SUCCESS, (uint8_t *)(para), lenth);
        face_task_set_mode(FACE_MODE_SLEEP);
        break;
    case FACE_RESULT_SUCCESS_REGISTER_UP:
        OB_LOGD(TAG, "FACE_RESULT_SUCCESS_REGISTER_UP");
        uart_msg_face(EVENT_CODE_FACE_REGISTER_UP, NULL, 0);
        break;
    case FACE_RESULT_SUCCESS_REGISTER_DOWN:
        OB_LOGD(TAG, "FACE_RESULT_SUCCESS_REGISTER_DOWN");
        uart_msg_face(EVENT_CODE_FACE_REGISTER_DOWN, NULL, 0);
        break;
    case FACE_RESULT_SUCCESS_REGISTER_LEFT:
        OB_LOGD(TAG, "FACE_RESULT_SUCCESS_REGISTER_LEFT");
        uart_msg_face(EVENT_CODE_FACE_REGISTER_LEFT, NULL, 0);
        break;
    case FACE_RESULT_SUCCESS_REGISTER_RIGHT:
        OB_LOGD(TAG, "FACE_RESULT_SUCCESS_REGISTER_RIGHT");
        uart_msg_face(EVENT_CODE_FACE_REGISTER_RIGHT, NULL, 0);
        break;
    case FACE_RESULT_FAIL_REPEAT:
        OB_LOGD(TAG, "FACE_RESULT_FAIL_REPEAT");
        uart_msg_face(EVENT_CODE_FACE_EXISTS, NULL, 0);
        break;
    case FACE_RESULT_FAIL_TIMEOUT:
        OB_LOGD(TAG, "FACE_RESULT_FAIL_TIMEOUT");
        uart_msg_face(EVENT_CODE_FACE_TIME_OUT, NULL, 0);
        break;
    default:
        OB_LOGE(TAG, "[%s] not default", __func__);
        break;
    }
    system_time_task_set_work_time(WORK_TIME_OUT_VAULE);
}

static void face_register_palm_event_callback(uint8_t result, void* para, uint8_t lenth) {
    // OB_LOGI(TAG, "register palm event: result[%u]", result);
    // OB_LOGI_DUMP((uint8_t*)(para), lenth);
    switch (result)
    {
    case FACE_RESULT_SUCCESS_REGISTER:
        OB_LOGD(TAG, "FACE_RESULT_SUCCESS_REGISTER");
        uart_msg_face(EVENT_CODE_FACE_REGISTER_SUCCESS, (uint8_t *)(para), lenth);
        face_task_set_mode(FACE_MODE_SLEEP);
        break;
    case FACE_RESULT_FAIL_REPEAT:
        OB_LOGD(TAG, "FACE_RESULT_FAIL_REPEAT");
        uart_msg_face(EVENT_CODE_FACE_EXISTS, NULL, 0);
        break;
    case FACE_RESULT_FAIL_TIMEOUT:
        OB_LOGD(TAG, "FACE_RESULT_FAIL_TIMEOUT");
        uart_msg_face(EVENT_CODE_FACE_TIME_OUT, NULL, 0);
        break;
    default:
        OB_LOGE(TAG, "[%s] not default", __func__);
        break;
    }
    system_time_task_set_work_time(WORK_TIME_OUT_VAULE);
}

static void face_delete_event_callback(uint8_t result, void* para, uint8_t lenth) {
    // OB_LOGI(TAG, "delete event: result[%u]", result);
    // OB_LOGI_DUMP((uint8_t*)(para), lenth);
    switch (result)
    {
    case FACE_RESULT_SUCCESS_DELETE:
        OB_LOGD(TAG, "FACE_RESULT_SUCCESS_DELETE");
        uart_msg_face(EVENT_CODE_FACE_DELETE_SUCCESS, (uint8_t *)(para), lenth);
        break;
    case FACE_RESULT_FAIL_DELETE:
        OB_LOGD(TAG, "FACE_RESULT_FAIL_DELETE");
        uart_msg_face(EVENT_CODE_FACE_DELETE_FAIL, NULL, 0);
        break;
    default:
        OB_LOGE(TAG, "[%s] not default event[%d]", __func__, result);
        break;
    }
    face_task_set_mode(FACE_MODE_IDLE);
}

static void face_reset_all_event_callback(uint8_t result, void* para, uint8_t lenth) {
    // OB_LOGI(TAG, "delete event: result[%u]", result);
    // OB_LOGI_DUMP((uint8_t*)(para), lenth);
    switch (result)
    {
    case FACE_RESULT_SUCCESS_DELETE:
        OB_LOGD(TAG, "FACE_RESULT_SUCCESS_DELETE");
        uart_msg_face(EVENT_CODE_FACE_DELETE_SUCCESS, (uint8_t *)(para), lenth);
        break;
    case FACE_RESULT_SUCCESS_DELETE_ALL:
        OB_LOGD(TAG, "FACE_RESULT_SUCCESS_DELETE");
        uart_msg_face(EVENT_CODE_FACE_DELETE_SUCCESS, (uint8_t *)(para), lenth);
        break;
    case FACE_RESULT_FAIL_DELETE:
        OB_LOGD(TAG, "FACE_RESULT_FAIL_DELETE");
        uart_msg_face(EVENT_CODE_FACE_DELETE_FAIL, NULL, 0);
        break;
    default:
        OB_LOGE(TAG, "[%s] not default event[%d]", __func__, result);
        break;
    }
    face_task_set_mode(FACE_MODE_IDLE);
}

#if (FACE_ENABLE_VERIFY_DELETE)
static void face_verify_delete_event_callback(uint8_t result, void* para, uint8_t lenth) {
    // OB_LOGI(TAG, "verify-delete event: result[%u]", result);
    // OB_LOGI_DUMP((uint8_t*)(para), lenth);
    switch (result)
    {
    case FACE_RESULT_SUCCESS_DELETE:
        OB_LOGD(TAG, "FACE_RESULT_SUCCESS_DELETE");
        uart_msg_face(EVENT_CODE_FACE_DELETE_SUCCESS, (uint8_t *)(para), lenth);
        break;
    case FACE_RESULT_FAIL_DELETE:
        OB_LOGD(TAG, "FACE_RESULT_FAIL_DELETE");
        uart_msg_face(EVENT_CODE_FACE_DELETE_FAIL, NULL, 0);
        break;
    default:
        OB_LOGE(TAG, "[%s] not default", __func__);
        break;
    }
    face_task_set_mode(FACE_MODE_IDLE);
}
#endif

void face_task_delete_face(face_delete_params_t params) {
    if (NULL == face_task_driver.io->set_mode) {
        return;
    }

    face_task_driver.io->set_mode(FACE_MODE_DELETE, face_delete_event_callback, (&params), sizeof(face_delete_params_t));
}

void face_task_reset_all_face(face_delete_params_t params) {
    if (NULL == face_task_driver.io->set_mode) {
        return;
    }

    face_task_driver.io->set_mode(FACE_MODE_RESET_ALL, face_reset_all_event_callback, (&params), sizeof(face_delete_params_t));
}

/**
 * @brief 读取人脸模块当前工作模式
 * @return 当前模式值（成功）/ 0xFF（失败，如指针为空）
 *         模式枚举参考：FACE_MODE_IDLE/FACE_MODE_VERIFY/FACE_MODE_REGISTER等
 */
uint8_t face_task_read_mode(void)
{
    // 1. 空指针校验：防止访问空指针导致崩溃
    if (face_task_driver.io == NULL || face_task_driver.io->read_mode == NULL)
    {
        return 0xFF; // 返回非法值标识读取失败
    }
    return face_task_driver.io->read_mode();
}

void face_task_set_mode(uint8_t mode) {
    if (NULL == face_task_driver.io->set_mode) {
        return;
    }
    switch (mode)
    {
        case FACE_MODE_IDLE:      // 空闲状态，不做检测
            face_task_driver.io->set_mode(mode, NULL, NULL, 0);
            break;
        case FACE_MODE_VERIFY:    // 验证模式
        case FACE_MODE_VERIFY_DEMO:
            face_task_driver.io->set_mode(mode, face_verify_event_callback, NULL, 0);
            break;
        case FACE_MODE_REGISTER:  // 注册模式
            face_task_driver.io->set_mode(mode, face_register_event_callback, NULL, 0);
            break;
#if (FACE_ENABLE_VERIFY_DELETE)
        case FACE_MODE_VERIFY_DELETE:     // 通过验证删除
            face_task_driver.io->set_mode(mode, face_verify_delete_event_callback, NULL, 0);
            break;
#endif
        case FACE_MODE_SLEEP:     // 休眠
            face_task_driver.io->set_mode(mode, NULL, NULL, 0);
            break;
        case FACE_MODE_REGISTER_PALM:  // 注册模式(掌静脉)
            face_task_driver.io->set_mode(mode, face_register_palm_event_callback, NULL, 0);
            break;
        default:
            break;
    }
}

void face_task_init(void) {

    face_task_driver.io = bsp_face_get_driver();

    if (NULL == face_task_driver.io->init) {
        return;
    }

    face_task_driver.io->init(face_init_event_callback);

    OB_LOGD(TAG, "init");
}

void face_task_loop(void) {
    if (NULL == face_task_driver.io->loop) {
        return;
    }

    if (face_task_driver.io->loop()) {
        face_task_set_mode(FACE_TASK_MODE_DEFAULT); // 唤醒切换至默认模式
    }
}

void face_task_set_attr(void* attr, uint8_t lenth) {
    if (NULL == face_task_driver.io->set_attr) {
        return;
    }

    face_function_attr_t face_attr;

    memcpy(&face_attr, attr, lenth);
    
    // user add...
    // read chip sn
    /*
    uint8_t buffer[14];
    memcpy(face_attr.chip_sn, buffer, 14);
    */

    face_task_driver.io->set_attr((void*)(&face_attr), lenth);
}
