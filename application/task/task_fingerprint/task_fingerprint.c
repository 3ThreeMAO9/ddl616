
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

#include "user.h"
#include "event.h"
#include "msg_protocol.h"
#include "parameter.h"
#include "lock_log.h"

#define OB_LOG_LEVEL OB_LOG_LEVEL_NONE
#include "ob_log.h"
#define TAG "task_fp"

/***************Variable***************/
static fp_task_driver_t fp_task_driver;

// ------------------------------------------
static uint8_t fp_init_event_callback(uint8_t event, void* params, uint8_t lenth) {
    // fp_module_attr_t* ptr = (fp_module_attr_t*)(params);

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
//    uint16_t* ptr = (uint16_t*)(params);

    switch (event)
    {
    case FP_EVENT_SUCCESS_HANDLE:
    {
        uint16_t finger_id = *(uint16_t*)params;
        uint8_t key_id = 0;
        
        OB_LOGI(TAG, "FP_EVENT_SUCCESS_HANDLE finger_id %u", finger_id);
        
        if (isValidKeyFingerprint(finger_id, &key_id)) {
            OB_LOGI(TAG, "matched key_id = %u", key_id);
            lock_log_add_record(KIOT_TM_P_RECORD_EVENT_TYPE_CAO_ZUO_JI_LU,
                                KIOT_TM_P_RECORD_OPERATION_TYPE_KAI_SUO_JI_LU,
                                KIOT_TM_P_RECORD_UNLOCK_TYPE_ZHI_WEN_KAI_SUO, key_id);
            userHandleEventPush(EVENT_RESULT_SUCCESS_VERIFY_USER, key_id);
        }
        else {
            userHandleEventPush(EVENT_RESULT_FAIL_INVALID, 0);
        }
        break;
    }
    case FP_EVENT_INVALID_FP:
        OB_LOGI(TAG, "FP_EVENT_VERIFY_FAIL");
        if(isEmptyKey(false)){
            userHandleEventPush(EVENT_RESULT_SUCCESS_VERIFY_USER, 0);
        }
        else{
            userHandleEventPush(EVENT_RESULT_FAIL_INVALID, 0);
        }
        break;
    case FP_EVENT_INVALID_MODULE:
        OB_LOGI(TAG, "FP_EVENT_INVALID_MODULE");
        userHandleEventPush(EVENT_RESULT_FAIL_INVALID, 0);
        break;
    case FP_EVENT_POWER_ON:
        OB_LOGI(TAG, "FP_EVENT_POWER_ON");
        break;
    default:
        OB_LOGE(TAG, "[%s] not default", __func__);
        break;
    }
    system_time_task_set_work_time(WORK_TIME_OUT_VAULE);
    // if (event != FP_EVENT_POWER_ON)
    //     face_task_set_mode(FACE_MODE_IDLE);
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
    {
        uint16_t finger_id = *(uint16_t*)params;
        uint8_t key_id = 0;

        OB_LOGI(TAG, "FP_EVENT_SUCCESS_HANDLE finger_id %u", finger_id);
        if (0 == finger_id)             // 模块 0 号 = 管理指纹（普通注册不会占 0 号，见 read_empty_fp_index）
        {
            modifyMasterKeyFinger(finger_id);
            lock_log_add_record(KIOT_TM_P_RECORD_EVENT_TYPE_CAO_ZUO_JI_LU,
                                KIOT_TM_P_RECORD_OPERATION_TYPE_TIAN_JIA_SHU_ZI_YAO_SHI,
                                0, 0);      // 添加密钥：管理指纹（key_id 0）
            userHandleEventPush(EVENT_RESULT_SUCCESS_ADD, finger_id);
        }
        else if (addKeyFinger(finger_id, &key_id)){
            OB_LOGI(TAG, "->key_id [%ld]", key_id);
            lock_log_add_record(KIOT_TM_P_RECORD_EVENT_TYPE_CAO_ZUO_JI_LU,
                                KIOT_TM_P_RECORD_OPERATION_TYPE_TIAN_JIA_SHU_ZI_YAO_SHI,
                                0, key_id);     // 添加密钥：指纹（key_id 0~49）
            userHandleEventPush(EVENT_RESULT_SUCCESS_ADD, finger_id);
        }
        else{
            userHandleEventPush(EVENT_RESULT_FAIL_ADD, 0);
        }
        break;
    }
    case FP_EVENT_CHIP_SN:
        // save chip sn
        writeFingerChipSn(ptr);
        break;
    case FP_EVENT_PROCESSING:
        OB_LOGI(TAG, "FP_EVENT_PROCESSING");
        userHandleEventPush(EVENT_RESULT_FINGERPRINT_PRESS, 0);
        break;
    case FP_EVENT_INVALID_REPEAT:
        OB_LOGI(TAG, "FP_EVENT_INVALID_REPEAT");
        break;
    case FP_EVENT_FAIL_COMBINE:
        OB_LOGI(TAG, "FP_EVENT_FAIL_COMBINE");
        userHandleEventPush(EVENT_RESULT_FAIL_ADD, 0);
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
    case FP_EVENT_POWER_ON:
        OB_LOGI(TAG, "FP_EVENT_POWER_ON");
        break;
    default:
        OB_LOGE(TAG, "[%s] not default", __func__);
        break;
    }
    system_time_task_set_work_time(WORK_TIME_OUT_VAULE);
    // face_task_set_mode(FACE_MODE_IDLE);
    return true;
}

#if (FP_ENABLE_DELETE)
// 待删的模块模板表（按模块模板号 page_id 索引；100 个模板 = 13 字节）+ 单条删除的进行状态。
// 模块同一时刻只认一条删除命令，且同模式重复下发会被丢弃（fp_set_mode_API 同模式早退），
// 所以 fp_task_delete_fp() 只记账，由 fp_task_loop() 在模块空闲时一条一条下发。
static uint8_t fp_del_pending[(FINGERPRINT_COUNT_MAX + 7) / 8];
static uint8_t fp_delete_busy   = false;    // 已下发一条，等模块终态
static uint8_t fp_delete_result = FP_DELETE_NONE;
static uint8_t fp_task_mode     = FP_MODE_IDLE;  // 任务层最后请求的模式（非 IDLE 时不抢模块）
static uint8_t fp_delete_notify = false;    // 本批有待删项：全部删完要补发一次完成事件
static uint8_t fp_delete_range_hint     = false;    // 有待下发的范围删除请求
static uint8_t fp_delete_range_inflight = false;    // 正在执行的这条是范围删除
static uint16_t fp_delete_range_page    = 0;        // 范围删除请求：起始模板号
static uint8_t  fp_delete_range_count   = 0;        // 范围删除请求：连续个数

static uint8_t fp_delete_event_callback(uint8_t event, void* params, uint8_t lenth) {
    // params 指向 ctx->params.del：只在回调里取值，不要等"下发下一条"之后再用
    //（下发会走 fp_is_ready → fingerprint_reset_context → memset ctx->params）
    fp_delete_params_t* ptr = (fp_delete_params_t*)(params);
    uint8_t is_range = fp_delete_range_inflight;        // 先记下来，下面要清标志

    switch (event)
    {
    case FP_EVENT_SUCCESS_HANDLE:
        fp_delete_result = FP_DELETE_OK;
        fp_delete_busy   = false;
        if (is_range) {
            fp_delete_range_inflight = false;
            memset(fp_del_pending, 0, sizeof(fp_del_pending));  // 范围删除成功：整批都删掉了
        }
        if (NULL != ptr) {
            if (is_range) {
                OB_LOGI(TAG, "FP_EVENT_SUCCESS_HANDLE   del_finger_id %u~%u",
                        ptr->page_id, ptr->page_id + ptr->count - 1);
            } else {
                OB_LOGI(TAG, "FP_EVENT_SUCCESS_HANDLE   del_finger_id %u", ptr->page_id);
            }
        }
        break;
    case FP_EVENT_FAIL_DELETE:
        fp_delete_result = FP_DELETE_FAIL;
        fp_delete_busy   = false;
        if (is_range) {
            fp_delete_range_inflight = false;   // 范围删除失败：待删表不动，退回逐条
            OB_LOGI(TAG, "del range failed, fallback per index");
        }
        OB_LOGI(TAG, "FP_EVENT_FAIL_DELETE");
        break;
    case FP_EVENT_POWER_ON:
        OB_LOGI(TAG, "FP_EVENT_POWER_ON");
        break;
    case FP_EVENT_DELETE_ALL:
        OB_LOGI(TAG, "FP_EVENT_DELETE_ALL");
        break;
    default:
        OB_LOGE(TAG, "[%s] not default event[%d]", __func__, event);
        break;
    }

    fp_task_set_mode(FP_MODE_IDLE);
    // face_task_set_mode(FACE_MODE_IDLE);
    system_time_task_set_work_time(WORK_TIME_OUT_VAULE);

    return true;
}

static uint8_t fp_reset_all_event_callback(uint8_t event, void* params, uint8_t lenth) {
//   fp_delete_params_t* ptr = (fp_delete_params_t*)(params);

    switch (event)
    {
    case FP_EVENT_SUCCESS_HANDLE:
        OB_LOGI(TAG, "FP_EVENT_SUCCESS_HANDLE");
        break;
    case FP_EVENT_DELETE_ALL:
        OB_LOGI(TAG, "FP_EVENT_DELETE_ALL");
        baseEventPush(Q_HANDLE_SIG, EVENT_RESULT_SUCCESS_DELETE_ALL);
        break;
    case FP_EVENT_FAIL_DELETE:
        OB_LOGI(TAG, "FP_EVENT_FAIL_DELETE");
        break;
    case FP_EVENT_POWER_ON:
        OB_LOGI(TAG, "FP_EVENT_POWER_ON");
        break;
    default:
        OB_LOGE(TAG, "[%s] not default event[%d]", __func__, event);
        break;
    }

    fp_task_set_mode(FP_MODE_IDLE);
    // if (event != FP_EVENT_DELETE_ALL)
    //     face_task_set_mode(FACE_MODE_IDLE);
    system_time_task_set_work_time(WORK_TIME_OUT_VAULE);

    return true;
}
#endif

#if (FP_ENABLE_DELETE)
static uint8_t fp_delete_has_pending(void) {
    uint8_t i;

    for (i = 0; i < sizeof(fp_del_pending); i++) {
        if (fp_del_pending[i]) {
            return true;
        }
    }

    return false;
}

void fp_task_delete_fp(fp_delete_params_t params) {
    uint16_t capacity = 0;
    uint16_t id;
    uint8_t  i;

    if (NULL == fp_task_driver.io->set_mode) {
        return;
    }

    // 按模块实际容量收敛 count（调用方可传 0xFF 表示"从 page_id 起删到最末"）
    if (NULL != fp_task_driver.io->get_capacity) {
        capacity = fp_task_driver.io->get_capacity();
    }
    if (0 == capacity) {
        capacity = FINGERPRINT_COUNT_MAX;
    }
    if (params.page_id >= capacity) {
        return;
    }
    if ((uint16_t)(params.page_id + params.count) > capacity) {
        params.count = (uint8_t)(capacity - params.page_id);
    }

    // 记账：这些模板号既是"要删的"，也是范围删除失败后的逐条回退清单
    for (i = 0; i < params.count; i++) {
        id = params.page_id + i;
        fp_del_pending[id >> 3] |= (1 << (id & 7));
    }

    if (params.count > 1) {
        // 连续多个：先按一条范围命令下发（失败时待删表还在，自动退回逐条）
        fp_delete_range_page  = params.page_id;
        fp_delete_range_count = params.count;
        fp_delete_range_hint  = true;
    }

    fp_delete_notify = true;        // 本批删完后要报一次完成
}

uint8_t fp_task_delete_result(void) {
    return fp_delete_result;
}

uint8_t fp_task_delete_is_done(void) {
    return (false == fp_delete_busy) && (false == fp_delete_has_pending());
}

// 模块空闲、且任务层没被别的流程（验证/注册…）占用时，下发下一条待删模板
static void fp_task_delete_process(void) {
    fp_delete_params_t params;
    uint16_t id;

    if (fp_delete_busy) {
        return;                     // 等这一条的终态
    }

    if (false == fp_delete_has_pending()) {
        fp_delete_range_hint = false;
        if (fp_delete_notify) {     // 本批删完了：补发一次完成事件（调用方据此才报"删除成功"）
            fp_delete_notify = false;
            baseEventPush(Q_HANDLE_SIG, EVENT_RESULT_DELETE_KEYS_DONE);
        }
        return;
    }

    if (FP_MODE_IDLE != fp_task_mode) {
        return;                     // 模块正被验证/注册占用，等它空出来再接着删
    }

    if (fp_delete_range_hint) {
        // 一条范围删除（page_id/count 已由 fp_task_delete_fp 按模块容量收敛好）。
        // 模块若因中间有空槽而失败，下面会自动退回逐条删除（待删表不动）
        fp_delete_range_hint = false;
        params.page_id   = fp_delete_range_page;
        params.count     = fp_delete_range_count;
        fp_delete_result = FP_DELETE_NONE;

        OB_LOGI(TAG, "del range: %u~%u", params.page_id, params.page_id + params.count - 1);

        if (false == fp_task_driver.io->set_mode(FP_MODE_DELETE, fp_delete_event_callback, (&params), sizeof(fp_delete_params_t))) {
            fp_delete_range_hint = true;        // 模块未就绪：留着下一轮再试
            return;
        }

        fp_delete_range_inflight = true;
        fp_delete_busy = true;
        fp_task_mode   = FP_MODE_DELETE;
        return;
    }

    for (id = 0; id < FINGERPRINT_COUNT_MAX; id++) {
        if (0 == (fp_del_pending[id >> 3] & (1 << (id & 7)))) {
            continue;
        }

        fp_del_pending[id >> 3] &= (uint8_t)~(1 << (id & 7));

        params.page_id   = id;
        params.count     = 1;
        fp_delete_result = FP_DELETE_NONE;

        if (false == fp_task_driver.io->set_mode(FP_MODE_DELETE, fp_delete_event_callback, (&params), sizeof(fp_delete_params_t))) {
            fp_del_pending[id >> 3] |= (1 << (id & 7));     // 模块未就绪：放回待删表，下一轮再试
            return;
        }

        fp_delete_busy = true;
        fp_task_mode   = FP_MODE_DELETE;
        return;
    }
}

void fp_task_reset_all_fp(fp_delete_params_t params) {
    if (NULL == fp_task_driver.io->set_mode) {
        return;
    }

    fp_task_driver.io->set_mode(FP_MODE_RESET_ALL, fp_reset_all_event_callback, (&params), sizeof(fp_delete_params_t));
}
#endif

void fp_task_set_mode(uint8_t mode) {
    if (NULL == fp_task_driver.io->set_mode) {
        return;
    }

#if (FP_ENABLE_DELETE)
    fp_task_mode = mode;        // 记录当前模式：待删模板只在 IDLE 时下发，不打断验证/注册
#endif

    switch (mode)
    {
        case FP_MODE_IDLE:      // 空闲状态，不做扫描
            fp_task_driver.io->set_mode(mode, NULL, NULL, 0);
            break;
        case FP_MODE_VERIFY:    // 验证模式
            fp_task_driver.io->set_mode(mode, fp_verify_event_callback, NULL, 0);
            break;
        case FP_MODE_REGISTER:  // 注册模式
        case FP_MODE_REGISTER_MASTER:   // 注册管理指纹（固定模块 0 号模板），回调/流程同注册
            fp_task_driver.io->set_mode(mode, fp_register_event_callback, NULL, 0);
            break;
        case FP_MODE_SLEEP:     // 休眠
            fp_task_driver.io->set_mode(mode, NULL, NULL, 0);
            break;
        
        default:
            break;
    }
}

void fp_task_init(void) {
    const fp_function_attr_t fp_attr = {
        .register_count = 5,
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

#if (FP_ENABLE_DELETE)
    fp_task_delete_process();       // 串行下发待删的指纹模板（一次一条）
#endif
}

void fp_task_set_attr(void* attr, uint8_t lenth) {
    if (NULL == fp_task_driver.io->set_attr) {
        return;
    }

    fp_function_attr_t fp_attr;

    memcpy(&fp_attr, attr, lenth);
    
    if (readFingerChipSn(fp_attr.chip_sn)){
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
