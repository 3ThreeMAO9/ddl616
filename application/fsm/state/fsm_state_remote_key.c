/**
 * Copyright (c) 2026 GZ-OB, All rights reserved.
 * File name: fsm_state_remote_key.c
 * Desc: 云端(0x02)发起的数字钥匙录入状态（指纹 / 卡片）
 * Version: 1.0.0
 * Revision: James_Zhang
 * Date: 2026-10-14
 *
 * 说明：云端 0x02 的 key_type 是"指纹/卡片"时，钥匙实体得等用户在锁上按压/刷卡，
 *       所以协议层先立即回 0x82（受理），再推 EVENT_RESULT_REMOTE_ADD_FINGER/CARD
 *       让待机态转到本文件的状态里：
 *         录入过程  -> 0x46 进度（task_fingerprint 里上报，已成功按压次数）
 *         录入成功  -> 0x47（user.c 落库时统一上报，带真实 key_id）
 *         失败/超时/被取消 -> 0x47（在这里上报，key_id 填 0xFF）
 *       与本地菜单里的录入状态（fsm_menu_add_normal_finger/nfc）的区别：这里结束回"待机"，
 *       不回菜单。
 *       管理指纹：APP 指定 user=0 + 指纹时（is_master_finger_pending），切 FP_MODE_REGISTER_MASTER
 *       （模块固定存 0 号模板），落库走 modifyMasterKeyFinger，0x47 由那里上报（key_id 0）。
 */

#include "fsm_state.h"
#include "state_inside.h"
#include "task_hmi.h"
#include "task_system_time.h"
#include "task_fingerprint.h"
#include "task_nfc.h"
#include "task_key.h"
#include "hal_rtc.h"
#include "msg_protocol.h"       // 0x47 事件上报

#include "user.h"
#include "event.h"
#include "key_event.h"

#define OB_LOG_LEVEL OB_LOG_LEVEL_DEFAULT
#include "ob_log.h"
#define TAG "fsm_remote_key"

/***************Function***************/

// 异常结束（失败/超时/取消）上报 0x47：成功那条由 user.c 落库时上报
static void remote_add_fail_report(uint8_t result, uint8_t type)
{
    uart_msg_event_key_add_complete(result, type, 0xFF, hal_get_rtc_time());
}

// ------------------------------------------
// 云端发起的指纹录入：录完回待机
QState lock_fsm_remote_add_finger(LockFsm *me, QEvent const *e)
{
    QState state = Q_IGNORED();

    switch (e->sig){
        case Q_ENTRY_SIG:
            keyEventInit();
            keyTaskHandle(KEY_TYPE_KEY_BOARD, true);           //key board
            // APP 指定录"管理指纹"时用管理注册模式（模块固定存 0 号模板），普通指纹从 1 号起
            fp_task_set_mode(is_master_finger_pending() ? FP_MODE_REGISTER_MASTER : FP_MODE_REGISTER);
            nfc_task_set_state(NFC_STATE_SLEEP);
            system_time_task_set_function_time(MENU_TIME_OUT_VAULE);
            hmiTaskSetState(HMI_STATE_ADD_NORMAL_FINGER);
            break;
        case Q_EXIT_SIG:
            set_master_finger_pending(false);       // 标记只在本次录入期间有效（成功路径在落库时已清）
            break;
        case Q_HANDLE_SIG:
            if (EVENT_RESULT_VOICE_TIME_OUT == e->dynamic_[0])
                system_time_task_set_function_time(MENU_TIME_OUT_VAULE);
            else if (EVENT_RESULT_CANCEL_ADD_KEY == e->dynamic_[0])     // APP 取消(0x03)
            {
                OB_LOGI(TAG, "remote add finger: cancelled");
                remote_add_fail_report(KIOT_TM_P_ADD_KEY_RESULT_TIAN_JIA_SHI_BAI,
                                       USER_TYPE_PERMANENT_FINGERPRINTS);
                state = Q_TRAN(lock_fsm_idle);
            }
            break;
        case Q_USER_HANDLE_SIG:
            if (EVENT_RESULT_SUCCESS_ADD == e->dynamic_[0])             // 成功：0x47 已由 user.c 上报
            {
                OB_LOGI(TAG, "remote add finger: OK");
                state = Q_TRAN(lock_fsm_idle);
            }
            else if (EVENT_RESULT_FAIL_ADD == e->dynamic_[0])
            {
                OB_LOGW(TAG, "remote add finger: fail");
                remote_add_fail_report(KIOT_TM_P_ADD_KEY_RESULT_TIAN_JIA_SHI_BAI,
                                       USER_TYPE_PERMANENT_FINGERPRINTS);
                state = Q_TRAN(lock_fsm_idle);
            }
            break;
        case Q_WORK_TIME_OUT_SIG:                                       // 系统工时到期：这里不做动作
            break;
        case Q_FUNCTION_TIME_OUT_SIG:                                   // 录入停留超时
            OB_LOGW(TAG, "remote add finger: timeout");
            remote_add_fail_report(KIOT_TM_P_ADD_KEY_RESULT_TIAN_JIA_CHAO_SHI,
                                   USER_TYPE_PERMANENT_FINGERPRINTS);
            state = Q_TRAN(lock_fsm_idle);
            break;
        default:
            break;
    }

    return state;
}

// ------------------------------------------
// 云端发起的卡片录入：刷完回待机
QState lock_fsm_remote_add_card(LockFsm *me, QEvent const *e)
{
    QState state = Q_IGNORED();

    switch (e->sig){
        case Q_ENTRY_SIG:
            keyEventInit();
            keyTaskHandle(KEY_TYPE_KEY_BOARD, true);           //key board
            fp_task_set_mode(FP_MODE_IDLE);
            nfc_task_set_state(NFC_STATE_REGISTER);
            system_time_task_set_function_time(MENU_TIME_OUT_VAULE);
            hmiTaskSetState(HMI_STATE_ADD_NORMAL_NFC);
            break;
        case Q_EXIT_SIG:
            break;
        case Q_HANDLE_SIG:
            if (EVENT_RESULT_VOICE_TIME_OUT == e->dynamic_[0])
                system_time_task_set_function_time(MENU_TIME_OUT_VAULE);
            else if (EVENT_RESULT_CANCEL_ADD_KEY == e->dynamic_[0])     // APP 取消(0x03)
            {
                OB_LOGI(TAG, "remote add card: cancelled");
                remote_add_fail_report(KIOT_TM_P_ADD_KEY_RESULT_TIAN_JIA_SHI_BAI,
                                       USER_TYPE_PERMANENT_CARD);
                state = Q_TRAN(lock_fsm_idle);
            }
            break;
        case Q_USER_HANDLE_SIG:
            if (EVENT_RESULT_SUCCESS_ADD == e->dynamic_[0])             // 成功：0x47 已由 user.c 上报
            {
                OB_LOGI(TAG, "remote add card: OK");
                state = Q_TRAN(lock_fsm_idle);
            }
            else if (EVENT_RESULT_FAIL_CARD_REPEAT == e->dynamic_[0])   // 这张卡已经录过
            {
                OB_LOGW(TAG, "remote add card: card exists");
                remote_add_fail_report(KIOT_TM_P_ADD_KEY_RESULT_MI_YAO_YI_CUN_ZAI,
                                       USER_TYPE_PERMANENT_CARD);
                state = Q_TRAN(lock_fsm_idle);
            }
            break;
        case Q_WORK_TIME_OUT_SIG:                                       // 系统工时到期：这里不做动作
            break;
        case Q_FUNCTION_TIME_OUT_SIG:                                   // 刷卡停留超时
            OB_LOGW(TAG, "remote add card: timeout");
            remote_add_fail_report(KIOT_TM_P_ADD_KEY_RESULT_TIAN_JIA_CHAO_SHI,
                                   USER_TYPE_PERMANENT_CARD);
            state = Q_TRAN(lock_fsm_idle);
            break;
        default:
            break;
    }

    return state;
}
