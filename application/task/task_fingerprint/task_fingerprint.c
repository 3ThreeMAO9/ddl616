
/**
 * Copyright (c) 2025 GZ-OB, All rights reserved.
 * File name: bsp_fingerprint.c
 * Desc: 指纹/ 指静脉模块任务层定义
 * Version: 1.0.0
 * Revision: XQ_R
 * Date: 2025-12-10
 */
#include "task_fingerprint.h"
#include "task_system_time.h"

#include "user_parameter.h"
#include "msg_protocol.h"

#define OB_LOG_LEVEL OB_LOG_LEVEL_DEFAULT
#include "ob_log.h"
#define TAG "task_fp"

/***************Variable***************/
static fp_task_driver_t fp_task_driver;

// ------------------------------------------
static uint8_t fp_init_event_callback(uint8_t event, void* params, uint8_t lenth) {
    fp_module_attr_t* ptr = (fp_module_attr_t*)(params);

    OB_LOGI(TAG, "init event[%u]", event);
    // OB_LOGI_DUMP((uint8_t*)(params), lenth);

    // user add...
    // send init event -> params: *ptr;

    // 同步指纹or指静脉类型，容量，指纹库(按bit对应)

    fp_task_set_mode(FP_MODE_IDLE); // 上电初始化完成，自动切至默认模式下

    return true;
}

static uint8_t fp_verify_event_callback(uint8_t event, void* params, uint8_t lenth)
{
    uint16_t* ptr = (uint16_t*)(params);

    switch (event)
    {
    case FP_EVENT_SUCCESS_HANDLE:
        OB_LOGI(TAG, "FP_EVENT_SUCCESS_HANDLE   add_finger_id %d", *ptr);
        uart_msg_finger(EVENT_CODE_FINGER_VERIFY_SUCCESS, *ptr);
        break;
    case FP_EVENT_INVALID_FP:
        OB_LOGI(TAG, "FP_EVENT_VERIFY_FAIL");
        uart_msg_finger(EVENT_CODE_FINGER_VERIFY_FAIL, 0);
        break;
    case FP_EVENT_INVALID_MODULE:
        OB_LOGI(TAG, "FP_EVENT_INVALID_MODULE");
        uart_msg_finger(EVENT_CODE_FINGER_VERIFY_FAIL_MODULE, 0);
        break;
    default:
        OB_LOGE(TAG, "[%s] not default", __func__);
        break;
    }
    system_time_task_set_work_time(WORK_TIME_OUT_VAULE);
    // fp_task_set_mode(FP_MODE_IDLE);
    return true;
}

static uint8_t fp_register_event_callback(uint8_t event, void* params, uint8_t lenth) {
    uint8_t* ptr = (uint8_t*)(params);

    OB_LOGI(TAG, "register event[%u]", event);
    OB_LOGI_DUMP(ptr, lenth);

    switch (event)
    {
    case FP_EVENT_SUCCESS_HANDLE:
        uart_msg_finger(EVENT_CODE_FINGER_REGISTER_SUCCESS, *ptr);
        break;
    case FP_EVENT_CHIP_SN:
        // save chip sn
        if (write_finger_module_chip_sn(ptr, lenth)) {
            OB_LOGI(TAG, "save finger module chip sn");
        }
        break;
    case FP_EVENT_PROCESSING:
        uart_msg_finger(EVENT_CODE_FINGER_REGISTER_STEP, *ptr);
        break;
    case FP_EVENT_INVALID_REPEAT:
        OB_LOGI(TAG, "FP_EVENT_INVALID_REPEAT");
        break;
    case FP_EVENT_FAIL_COMBINE:
        OB_LOGI(TAG, "FP_EVENT_FAIL_COMBINE");
        break;
    case FP_EVENT_FAIL_FULL:
        OB_LOGI(TAG, "FP_EVENT_FAIL_FULL");
        break;
    case FP_EVENT_FAIL_READ_SN:
        OB_LOGI(TAG, "FP_EVENT_FAIL_READ_SN");
        break;
    case FP_EVENT_FAIL_STORE:
        OB_LOGI(TAG, "FP_EVENT_FAIL_STORE");
        break;
    default:
        OB_LOGE(TAG, "[%s] not default", __func__);
        break;
    }
    system_time_task_set_work_time(WORK_TIME_OUT_VAULE);

    return true;
}

#if (FP_ENABLE_DELETE)
static uint8_t fp_delete_event_callback(uint8_t event, void* params, uint8_t lenth) {
    fp_delete_params_t* ptr = (fp_delete_params_t*)(params);

    switch (event)
    {
    case FP_EVENT_SUCCESS_HANDLE:
        OB_LOGI(TAG, "FP_EVENT_SUCCESS_HANDLE   del_finger_id %d", ptr->page_id);
        uart_msg_finger(EVENT_CODE_FINGER_DELETE_SUCCESS, ptr->page_id);
        break;
    case FP_EVENT_FAIL_DELETE:
        OB_LOGI(TAG, "FP_EVENT_FAIL_DELETE");
        uart_msg_finger(EVENT_CODE_FINGER_DELETE_FAIL, 0);
        break;
    default:
        OB_LOGE(TAG, "[%s] not default", __func__);
        break;
    }

    fp_task_set_mode(FP_MODE_IDLE);
    system_time_task_set_work_time(WORK_TIME_OUT_VAULE);

    return true;
}
#endif

#if (FP_ENABLE_VERIFY_DELETE)
static uint8_t fp_verify_delete_event_callback(uint8_t event, void* params, uint8_t lenth)
{
    fp_delete_params_t *ptr = (fp_delete_params_t*)(params);
    switch (event)
    {
    case FP_EVENT_SUCCESS_HANDLE:
        OB_LOGI(TAG, "FP_EVENT_SUCCESS_HANDLE   del_finger_id %d", ptr->page_id);
        uart_msg_finger(EVENT_CODE_FINGER_DELETE_SUCCESS, ptr->page_id);
        break;
    case FP_EVENT_INVALID_FP:
        OB_LOGI(TAG, "FP_EVENT_INVALID_FP");
        uart_msg_finger(EVENT_CODE_FINGER_DELETE_INVALID_FP, 0);
        break;
    case FP_EVENT_FAIL_DELETE:
        OB_LOGI(TAG, "FP_EVENT_FAIL_DELETE");
        uart_msg_finger(EVENT_CODE_FINGER_DELETE_FAIL, 0);
        break;
    default:
        OB_LOGE(TAG, "[%s] not default", __func__);
        break;
    }

    // fp_task_set_mode(FP_MODE_IDLE);
    system_time_task_set_work_time(WORK_TIME_OUT_VAULE);

    return true;
}
#endif

#if (FP_ENABLE_DELETE)
void fp_task_delete_fp(fp_delete_params_t params) {
    if (NULL == fp_task_driver.io->set_mode) {
        return;
    }

    fp_task_driver.io->set_mode(FP_MODE_DELETE, fp_delete_event_callback, (&params), sizeof(fp_delete_params_t));
}
#endif

void fp_task_set_mode(uint8_t mode) {
    if (NULL == fp_task_driver.io->set_mode) {
        return;
    }
    
    switch (mode)
    {
        case FP_MODE_IDLE:      // 空闲状态，不做扫描
            fp_task_driver.io->set_mode(mode, NULL, NULL, 0);
            break;
        case FP_MODE_VERIFY:    // 验证模式
            fp_task_driver.io->set_mode(mode, fp_verify_event_callback, NULL, 0);
            break;
        case FP_MODE_REGISTER:  // 注册模式
            fp_task_driver.io->set_mode(mode, fp_register_event_callback, NULL, 0);
            break;
#if (FP_ENABLE_VERIFY_DELETE)
        case FP_MODE_VERIFY_DELETE:     // 通过验证删除
            fp_task_driver.io->set_mode(mode, fp_verify_delete_event_callback, NULL, 0);
            break;
#endif
        case FP_MODE_SLEEP:     // 休眠
            fp_task_driver.io->set_mode(mode, NULL, NULL, 0);
            break;
        
        default:
            break;
    }
}

void fp_task_init(void) {
    const fp_function_attr_t fp_attr = {
        .register_count = 6,
        .repeat = 0,
        .auto_sleep = 0,
        .idle_irq = 1,
        .led_default = FP_LED_BLUE,
    };

    fp_task_driver.io = bsp_fp_get_driver();

    if (NULL == fp_task_driver.io->init) {
        return;
    }

    fp_task_driver.io->init(fp_init_event_callback);

    fp_task_set_attr((void*)(&fp_attr), sizeof(fp_function_attr_t));

    OB_LOGD(TAG, "init");
}

void fp_task_loop(void) {
    if (NULL == fp_task_driver.io->loop) {
        return;
    }

    if (fp_task_driver.io->loop()) {
        
    }
}

void fp_task_set_attr(void* attr, uint8_t lenth) {
    if (NULL == fp_task_driver.io->set_attr) {
        return;
    }

    fp_function_attr_t fp_attr;

    memcpy(&fp_attr, attr, lenth);
    
    if (read_finger_module_chip_sn(fp_attr.chip_sn)){
        fp_attr.chip_sn_flag = 1;
        OB_LOGD(TAG, "read fp chip sn->");
        OB_LOGD_DUMP(fp_attr.chip_sn, 14);
    }

    fp_task_driver.io->set_attr((void*)(&fp_attr), lenth);
}

uint8_t fp_task_is_wake(void) {
    if (NULL == fp_task_driver.io->is_wake) {
        return false;
    }

    if (fp_task_driver.io->is_wake()){
        return WAKE_SOURCE_FINGER;
    }
    return WAKE_SOURCE_NULL;
}

uint8_t fp_task_is_busy(void) {
    if (NULL == fp_task_driver.io->is_busy) {
        return false;
    }

    return fp_task_driver.io->is_busy();
}

#if (FP_ENABLE_LED_CONTROL)
uint8_t fp_task_control_led(uint8_t color) {
    if (NULL == fp_task_driver.io->control_led) {
        return false;
    }

    return fp_task_driver.io->control_led(color);
}
#endif

uint8_t fp_task_test_mode(uint8_t handle_id) {
    const fp_delete_params_t delete_params = {
        .page_id = 0,
        .count = 10,
    };
    const fp_function_attr_t fp_func_attr = {
        .register_count = 6,
        .repeat = 0,
        .auto_sleep = 0,
        .idle_irq = 0,
        .led_default = 0,
    };

    OB_LOGD(TAG, "fp test mode[%u]", handle_id);

    switch (handle_id) {
        case 0:
            fp_task_set_mode(FP_MODE_SLEEP);
            break;
        case 1:
            fp_task_set_mode(FP_MODE_IDLE);
            break;
        case 2:
            fp_task_set_mode(FP_MODE_VERIFY);
            break;
        case 3:
            fp_task_set_mode(FP_MODE_REGISTER);
            break;
        case 4:
            fp_task_set_mode(FP_MODE_VERIFY_DELETE);
            break;
        case 5:
            fp_task_delete_fp(delete_params);
            break;
        case 6:
            fp_task_set_attr((void*)(&fp_func_attr), sizeof(fp_function_attr_t));
            break;
        default:
            return false;
    }
		
		return true;
}
