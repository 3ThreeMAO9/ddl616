#include "fsm_state.h"
#include "task_system_time.h"
#include "key_event.h"
#include "state_inside.h"
#include "task_fingerprint.h"
#include "task_key.h"
#include "task_motor.h"
#include "task_hmi.h"
#include "parameter.h"

#define OB_LOG_LEVEL OB_LOG_LEVEL_DEBUG
#include "ob_log.h"
#define TAG "fsm_result"
/***************Variable***************/


/***************Function***************/


// ------------------------------------------

static QState lockFsmSuccessDeal(LockFsm *me, QEvent const *e, uint8_t hmiState)
{
    uint32_t keepTimeOut;
    QState state = Q_IGNORED();     //  没有对应事件就返回Q_IGNORED()

#if (Enabled==PRINTF_FSM)
    OB_LOGD(TAG, "Fsm_state[%s], Event[%u, %u]", "lockFsmSuccessDeal", e->sig, e->dynamic_[0]);
#endif
    switch (e->sig)
    {
        case Q_ENTRY_SIG:
            //keyTaskHandle(KEY_TYPE_KEY_BOARD, false);           //key board
            fp_task_set_mode(FP_MODE_IDLE);
            
            if (HMI_STATE_IDLE != hmiState)
            {
                keepTimeOut = hmiTaskSetState(hmiState);
            }
            else
            {
                keepTimeOut = 100;
            }

            // if (HMI_STATE_LOCK_SUCCESS != hmiState)
            // {
            //     me->verify_fail_cnt = 0;
            //     setUserParameter(USER_PARA_VERIFY_FAIL_CNT_ID, me->verify_fail_cnt);
            // }
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

#if (Enabled==PRINTF_FSM)
    OB_LOGD(TAG, "Fsm_state[%s], Event[%u, %u]", "lockFsmFailDeal", e->sig, e->dynamic_[0]);
#endif
    switch (e->sig)
    {
        case Q_ENTRY_SIG:
            keepTimeOut = hmiTaskSetState(hmiState);
            system_time_task_set_work_time(keepTimeOut);
            keyEventInit();
            fp_task_set_mode(FP_MODE_IDLE);

            // if ((errCntFlag) && ((me->verify_fail_cnt) < VERIFY_FAIL_CNT_FOR_SYSTEM_LOCK))
            // {
            //     (me->verify_fail_cnt)++;
            //     setUserParameter(USER_PARA_VERIFY_FAIL_CNT_ID, me->verify_fail_cnt);
            // }
            break;
        case Q_EXIT_SIG:
            break;
        case Q_WORK_TIME_OUT_SIG:
            // if(NULL != (me->branch))
            // {
            //     if ((me->verify_fail_cnt) >= VERIFY_FAIL_CNT_FOR_SYSTEM_LOCK)
            //     {
            //         state = Q_TRAN(lockFsmSystemLock);
            //     }
            //     else
            //     {
            //         state = Q_TRAN(me->branch);
            //     }
            // }
            // else
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

QState lockFsmHandleSuccessKeepGreen(LockFsm *me, QEvent const *e)
{
#if (Enabled==PRINTF_FSM)
    OB_LOGD(TAG, "Now State[handle success], Event[%d, %d]--", e->sig, e->dynamic_[0]);
#endif
    return lockFsmSuccessDeal(me, e, HMI_STATE_HANDLE_SUCCESS_KEEP_GREEN);
}

QState lockFsmHandleSuccess(LockFsm *me, QEvent const *e)
{
#if (Enabled==PRINTF_FSM)
    OB_LOGD(TAG, "Now State[handle success], Event[%d, %d]--", e->sig, e->dynamic_[0]);
#endif
    return lockFsmSuccessDeal(me, e, HMI_STATE_HANDLE_SUCCESS);
}

QState lockFsmVerifyAdminSuccess(LockFsm *me, QEvent const *e)
{
#if (Enabled==PRINTF_FSM)
    OB_LOGD(TAG, "Now State[verify admin success], Event[%d, %d]--", e->sig, e->dynamic_[0]);
#endif
    return lockFsmSuccessDeal(me, e, HMI_STATE_VERIFY_SUCCESS);
}

QState lockFsmVerifyUserSuccess(LockFsm *me, QEvent const *e)
{
#if (Enabled==PRINTF_FSM)
    OB_LOGD(TAG, "Now State[verify user success], Event[%d, %d]--", e->sig, e->dynamic_[0]);
#endif
    return lockFsmSuccessDeal(me, e, HMI_STATE_VERIFY_SUCCESS);
}

QState lockFsmUnlockSuccess(LockFsm *me, QEvent const *e)
{
#if (Enabled==PRINTF_FSM)
    OB_LOGD(TAG, "Now State[unlock success], Event[%d, %d]--", e->sig, e->dynamic_[0]);
#endif
    return lockFsmSuccessDeal(me, e, HMI_STATE_UNLOCK_SUCCESS);
}

QState lockFsmHandleFail(LockFsm *me, QEvent const *e)
{
#if (Enabled==PRINTF_FSM)
    OB_LOGD(TAG, "Now State[handle fail], Event[%d, %d]--", e->sig, e->dynamic_[0]);
#endif
    return lockFsmFailDeal(me, e, HMI_STATE_HANDLE_FAIL, false);
}

QState lockFsmHandleFailKeepRed(LockFsm *me, QEvent const *e)
{
#if (Enabled==PRINTF_FSM)
    OB_LOGD(TAG, "Now State[handle fail keep red], Event[%d, %d]--", e->sig, e->dynamic_[0]);
#endif
    return lockFsmFailDeal(me, e, HMI_STATE_HANDLE_FAIL_KEEP_RED, false);
}

QState lockFsmTimeOut(LockFsm *me, QEvent const *e)
{
#if (Enabled==PRINTF_FSM)
    OB_LOGD(TAG, "Now State[Time out], Event[%d, %d]--", e->sig, e->dynamic_[0]);
#endif
    return lockFsmFailDeal(me, e, HMI_STATE_TIME_OUT, false);
}

QState lockFsmVerifyFail(LockFsm *me, QEvent const *e)
{
#if (Enabled==PRINTF_FSM)
    OB_LOGD(TAG, "Now State[Verify fail], Event[%d, %d]--", e->sig, e->dynamic_[0]);
#endif
    return lockFsmFailDeal(me, e, HMI_STATE_HANDLE_FAIL, true);
}
