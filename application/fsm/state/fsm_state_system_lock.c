#include "fsm_state.h"
#include "state_inside.h"
#include "task_hmi.h"
#include "task_system_time.h"
#include "task_fingerprint.h"
#include "task_key.h"
#include "task_nfc.h"

#include "event.h"
#include "key_event.h"
#include "user.h"
#include "parameter.h"
#include "flash_data.h"

#define OB_LOG_LEVEL OB_LOG_LEVEL_NONE
#include "ob_log.h"
#define TAG "fsm_system_lock"

/***************Variable***************/


/***************Function***************/


// ------------------------------------------

QState lock_fsm_system_lock(LockFsm *me, QEvent const *e)
{
    QState state = Q_IGNORED();     //  没有对应事件就返回Q_IGNORED()

    OB_LOGD(TAG, "Fsm_state[%s], Event[%u, %u]", "system_lock", e->sig, e->dynamic_[0]);

    switch (e->sig){
        case Q_ENTRY_SIG:
            keyTaskHandle(KEY_TYPE_KEY_BOARD, true);           //key board
            fp_task_set_mode(FP_MODE_VERIFY);
            nfc_task_set_state(NFC_STATE_VERIFY);

            if (Disabled == readUserParameter(USER_PARA_SYSTEM_LOCK_ID))
            {
                setUserParameter(USER_PARA_SYSTEM_LOCK_ID, Enabled);
            }
            me->verify_fail_time = 0;
            me->verify_fail_cnt= 0;
            setUserParameter(USER_PARA_VERIFY_FAIL_CNT_ID, me->verify_fail_cnt);

            hmiTaskSetAllowSelintFlag(false);
            system_time_task_set_work_time(SYSTEM_LOCK_TIME);
            system_time_task_set_function_time(SYSTEM_LOCK_TIMEOUT);
            break;
        case Q_EXIT_SIG:
            setUserParameter(USER_PARA_SYSTEM_LOCK_ID, Disabled);
            break;
        case Q_KEY_BOARD_PRESS_SIG:
            hmiTaskSetState(HMI_STATE_KEY_BOARD_LED_ON);
            if (KEY_NUM_13 == e->dynamic_[0])   // 门铃
            {

            }
            else
            {
                keyEventVerifyUser(e->dynamic_[0]);
                hmiTaskSetState(HMI_STATE_KEY_BOARD_PRESS);
            }
            break;
        case Q_HANDLE_SIG:
            if ((EVENT_RESULT_FAIL_TOO_SHORT == e->dynamic_[0]) || (EVENT_RESULT_FAIL_TOO_LONG == e->dynamic_[0]))
            {
                hmiTaskSetState(HMI_STATE_SYSTEM_LOCK);
                system_time_task_set_function_time(SYSTEM_LOCK_TIMEOUT);
            }
            else if (EVENT_RESULT_BREAK_LONG_PRESS == e->dynamic_[0])
            {
                keyTaskHandle(KEY_TYPE_TAMPER_KEY, false);
                hmi_task_tamper_warn_time(TAMPER_WARN_KEEP_TIME);
            }
            else if (EVENT_RESULT_BREAK_WARN == e->dynamic_[0])
            {
                hmiTaskSetState(HMI_STATE_TAMPER_WARN);
            }
            else if (EVENT_RESULT_VOICE_TIME_OUT == e->dynamic_[0])
            {
                system_time_task_set_function_time(SYSTEM_LOCK_TIMEOUT);    // 语音播报完成后重置时间
            }
            else if (EVENT_RESULT_RESET == e->dynamic_[0])
            {
                state = Q_TRAN(lock_fsm_reset);
            }
            break;
        case Q_USER_HANDLE_SIG:
            if (EVENT_RESULT_SUCCESS_VERIFY_USER == (e->dynamic_[0]))
            {
                hmiTaskSetState(HMI_STATE_SYSTEM_LOCK);
                system_time_task_set_function_time(SYSTEM_LOCK_TIMEOUT);
            }
            else if (EVENT_RESULT_FAIL_INVALID == (e->dynamic_[0]))
            {
                hmiTaskSetState(HMI_STATE_SYSTEM_LOCK);
                system_time_task_set_function_time(SYSTEM_LOCK_TIMEOUT);
            }
            break;
        case Q_FUNCTION_TIME_OUT_SIG:
            keyEventInit();
            hmiTaskSetState(HMI_STATE_KEY_BOARD_LED_OFF);
            break;
        case Q_WORK_TIME_OUT_SIG:
            state = Q_TRAN(lock_fsm_sleep);
            break;
        default:
            break;
    }

    return state;
}
