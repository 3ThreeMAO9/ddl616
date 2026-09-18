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
#include "flash_data.h"

#define OB_LOG_LEVEL OB_LOG_LEVEL_NONE
#include "ob_log.h"
#define TAG "fsm_low_power_system_lock"

/***************Variable***************/


/***************Function***************/


// ------------------------------------------

QState lock_fsm_low_power_system_lock(LockFsm *me, QEvent const *e)
{
    QState state = Q_IGNORED();     //  没有对应事件就返回Q_IGNORED()

    OB_LOGD(TAG, "Fsm_state[%s], Event[%u, %u]", "low_power_system_lock", e->sig, e->dynamic_[0]);

    switch (e->sig){
        case Q_ENTRY_SIG:
            keyTaskHandle(KEY_TYPE_KEY_BOARD, false);           //key board
            nfc_task_set_state(NFC_STATE_IDLE);
            fp_task_set_mode(FP_MODE_IDLE);

            hmiTaskSetAllowSelintFlag(false);
            hmiTaskSetState(HMI_STATE_ALL_LED_OFF);

            system_time_task_set_work_time(WORK_WAKE_TIME_OUT_VAULE);
            break;
        case Q_EXIT_SIG:
            break;
        case Q_HANDLE_SIG:
        case Q_USER_HANDLE_SIG:
        case Q_FUNCTION_TIME_OUT_SIG:
        case Q_WORK_TIME_OUT_SIG:
            state = Q_TRAN(lock_fsm_sleep);
            break;
        default:
            break;
    }

    return state;
}
