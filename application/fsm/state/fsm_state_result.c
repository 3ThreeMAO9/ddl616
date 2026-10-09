#include "fsm_state.h"
#include "task_system_time.h"
#include "key_event.h"
#include "state_inside.h"
#include "task_fingerprint.h"
#include "task_key.h"
#include "task_motor.h"
#include "task_hmi.h"
#include "task_nfc.h"
#include "task_battery.h"

#include "parameter.h"
#include "user.h"
#include "lock_log.h"

#define OB_LOG_LEVEL OB_LOG_LEVEL_NONE
#include "ob_log.h"
#define TAG "fsm_result"

// 状态切换的调试打印统一在引擎里（qp_frame/qp_port.c 的 FSM_DISPATCH_LOG），本文件不再各写一行

/***************Variable***************/

/***************Function***************/

// ------------------------------------------

static QState lockFsmSuccessDeal(LockFsm *me, QEvent const *e, uint8_t hmiState)
{
    uint32_t keepTimeOut;
    QState state = Q_IGNORED();     //  没有对应事件就返回Q_IGNORED()

    switch (e->sig)
    {
        case Q_ENTRY_SIG:
#if (Enabled == PRINTF_FSM)
            OB_LOGD(TAG, "branch[%08X]--", me->branch);
#endif
            keyTaskHandle(KEY_TYPE_KEY_BOARD, false);           //key board
            fp_task_set_mode(FP_MODE_IDLE);
            nfc_task_set_state(NFC_STATE_SLEEP);
            if (HMI_STATE_IDLE != hmiState)
            {
                keepTimeOut = hmiTaskSetState(hmiState);
            }
            else
            {
                keepTimeOut = 100;
            }

            if (HMI_STATE_LOCK_SUCCESS != hmiState)
            {
                me->verify_fail_cnt = 0;
                me->verify_fail_time = 0;
                setUserParameter(USER_PARA_VERIFY_FAIL_CNT_ID, me->verify_fail_cnt);
            }
            system_time_task_set_work_time(keepTimeOut);
            break;

        case Q_EXIT_SIG:
            break;
        case Q_HANDLE_SIG:
            if (EVENT_RESULT_SUCCESS_UNLOCKED == e->dynamic_[0])
            {
                keepTimeOut = hmiTaskSetState(HMI_STATE_UNLOCK_SUCCESS);
                system_time_task_set_work_time(keepTimeOut);
            }
            else if (EVENT_RESULT_SUCCESS_LOCKED == e->dynamic_[0])
            {
                keepTimeOut = hmiTaskSetState(HMI_STATE_NULL_IDLE);
                system_time_task_set_work_time(keepTimeOut);
            }
            break;
        case Q_WORK_TIME_OUT_SIG:
            if(NULL != (me->branch))
            {
                state = Q_TRAN(me->branch);
            }
            else
            {
                state = Q_TRAN(lock_fsm_idle);
#if (Enabled==PRINTF_ERR)
                OB_LOGE(TAG, "err: me->branch is null");
#endif
            }
            break;
        default:
            break;
    }

    return state;
}

static QState lockFsmFailDeal(LockFsm *me, QEvent const *e, uint8_t hmiState, uint8_t errCntFlag)
{  
    uint32_t keepTimeOut;
    QState state = Q_IGNORED();     //  没有对应事件就返回Q_IGNORED()

    switch (e->sig)
    {
        case Q_ENTRY_SIG:
#if (Enabled == PRINTF_FSM)
            OB_LOGD(TAG, "branch[%08X]--", me->branch);
#endif
            keyEventInit();
            keyTaskHandle(KEY_TYPE_KEY_BOARD, false);           //key board
            fp_task_set_mode(FP_MODE_IDLE);
            nfc_task_set_state(NFC_STATE_SLEEP);
            keepTimeOut = hmiTaskSetState(hmiState);
            system_time_task_set_work_time(keepTimeOut);
            OB_LOGW(TAG, "keepTimeOut[%ld]", keepTimeOut);
            if ((errCntFlag) && ((me->verify_fail_cnt) < VERIFY_FAIL_CNT_FOR_SYSTEM_LOCK))
            {
                if (me->verify_fail_cnt == 0)
                    me->verify_fail_time = hal_get_rtc_work_time();

                if (hal_get_rtc_work_time() > ((me->verify_fail_time) + VERIFY_FAIL_CNT_TIMEOUT))
                {
                    me->verify_fail_time = hal_get_rtc_work_time();
                    me->verify_fail_cnt = 0;
                }

                (me->verify_fail_cnt)++;
                setUserParameter(USER_PARA_VERIFY_FAIL_CNT_ID, me->verify_fail_cnt);
                if (me->verify_fail_cnt >= VERIFY_FAIL_CNT_FOR_WARN)
                    hmiTaskSetState(HMI_STATE_VERIFY_FAIL_WARN);
                OB_LOGW(TAG, "verify_fail_cnt[%ld]", me->verify_fail_cnt);
            }
            break;
        case Q_EXIT_SIG:
            OB_LOGW(TAG, "Q_EXIT_SIG");
            break;
        case Q_WORK_TIME_OUT_SIG:
            OB_LOGW(TAG, "Q_WORK_TIME_OUT_SIG");
            if(NULL != (me->branch))
            {
                if ((me->verify_fail_cnt) >= VERIFY_FAIL_CNT_FOR_SYSTEM_LOCK)
                {
                    lock_log_add_record(KIOT_TM_P_RECORD_EVENT_TYPE_BAO_JING_JI_LU,
                                        KIOT_TM_P_RECORD_ALARM_TYPE_SUO_DING_BAO_JING_SHU_RU_CUO_WU_MI_MA_HUO_ZHI_WEN_HUO_KA_PIAN_CHAO_GUO_5_CI_JIU_HUI_XI_TONG_SUO_DING_BAO_JING,
                                        0, 0);      // 系统锁定报警
                    state = Q_TRAN(lock_fsm_system_lock);
                }
                else
                {
                    state = Q_TRAN(me->branch);
                }
            }
            else
            {
                state = Q_TRAN(lock_fsm_idle);
#if (Enabled==PRINTF_ERR)
                OB_LOGE(TAG, "err: me->branch is null");
#endif
            }
            break;
        default:
            break;
    }

    return state;
}

QState lockFsmHandleAddSuccess(LockFsm *me, QEvent const *e)
{
    return lockFsmSuccessDeal(me, e, HMI_STATE_HANDLE_ADD_SUCCESS);
}

// ---- 删除普通用户的几种结果语音（播完回 me->branch，调用方指向删除用户菜单）----
QState lockFsmDeleteUserSuccess(LockFsm *me, QEvent const *e)
{
    return lockFsmSuccessDeal(me, e, HMI_STATE_DELETE_SUCCESS);
}

QState lockFsmDeleteUserFailAdmin(LockFsm *me, QEvent const *e)
{
    return lockFsmFailDeal(me, e, HMI_STATE_DELETE_FAIL_ADMIN, false);
}

QState lockFsmDeleteUserFailNotExist(LockFsm *me, QEvent const *e)
{
    return lockFsmFailDeal(me, e, HMI_STATE_DELETE_FAIL_NOT_EXIST, false);
}

QState lockFsmDeleteUserFailEmpty(LockFsm *me, QEvent const *e)
{
    return lockFsmFailDeal(me, e, HMI_STATE_DELETE_FAIL_EMPTY, false);
}

QState lockFsmDeleteUserFailTimeOut(LockFsm *me, QEvent const *e)
{
    return lockFsmFailDeal(me, e, HMI_STATE_DELETE_FAIL_TIME_OUT, false);
}

QState lockFsmHandleWakeUpSuccess(LockFsm *me, QEvent const *e)
{
    return lockFsmSuccessDeal(me, e, HMI_STATE_KEY_BOARD_WAKE_UP);
}

QState lockFsmHandleVoiceModeSuccess(LockFsm *me, QEvent const *e)
{
    return lockFsmSuccessDeal(me, e, HMI_STATE_HANDLE_VOICE_SUCCESS);
}

QState lockFsmHandleSuccess(LockFsm *me, QEvent const *e)
{
    return lockFsmSuccessDeal(me, e, HMI_STATE_HANDLE_SUCCESS);
}

QState lockFsmVerifyUserSuccess(LockFsm *me, QEvent const *e)
{
    uint8_t state = HMI_STATE_VERIFY_SUCCESS;   // 正常验证成功

    if (isEmptyKey(false))
        state = HMI_STATE_DEMO_VERIFY_SUCCESS;  // 体验模式验证成功

    if (isBatteryLow())
        state = HMI_STATE_LOW_PWOER_VERIFY_SUCCESS; // 低电验证成功

    return lockFsmSuccessDeal(me, e, state);
}

QState lockFsmHandleFail(LockFsm *me, QEvent const *e)
{
    return lockFsmFailDeal(me, e, HMI_STATE_HANDLE_FAIL, false);
}

QState lockFsmHandleCardRepeat(LockFsm *me, QEvent const *e)
{
    return lockFsmFailDeal(me, e, HMI_STATE_CARD_REPEAT, false);
}

QState lockFsmHandleAddFail(LockFsm *me, QEvent const *e)
{
    return lockFsmFailDeal(me, e, HMI_STATE_ENROLLMENT_FAIL, false);
}

QState lockFsmVerifyFail(LockFsm *me, QEvent const *e)
{
    return lockFsmFailDeal(me, e, HMI_STATE_VERIFY_FAIL, true);
}

QState lockFsmInputError(LockFsm *me, QEvent const *e)
{
    return lockFsmFailDeal(me, e, HMI_STATE_INPUT_ERROR, false);
}

