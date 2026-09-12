#include "fsm_state.h"
#include "state_inside.h"
#include "task_hmi.h"
#include "task_system_time.h"
#include "task_key.h"
#include "task_nfc.h"
#include "task_fingerprint.h"
#include "task_battery.h"

#include "parameter.h"

#define OB_LOG_LEVEL OB_LOG_LEVEL_DEFAULT
#include "ob_log.h"
#define TAG "fsm_power_on"

/***************Variable***************/


/***************Function***************/


// ------------------------------------------

static QState lock_fsm_power_on(LockFsm *me, QEvent const *e)
{
    QState state = Q_IGNORED();     //  没有对应事件就返回Q_IGNORED()

    OB_LOGD(TAG, "Fsm_state[%s], Event[%u, %u]", "power_on", e->sig, e->dynamic_[0]);

    switch (e->sig){
        case Q_ENTRY_SIG:
            keyEventInit();
            keyTaskHandle(KEY_TYPE_KEY_BOARD, false);           //key board
            fp_task_set_mode(FP_MODE_SLEEP);
            nfc_task_set_state(NFC_STATE_SLEEP);

            system_time_task_set_function_time(1200);
            system_time_task_set_work_time(1000);
            hmiTaskSetAllowSelintFlag(true);
            if (BATTERY_STATE_LOW_SYSTEM_LOCK == batteryTaskReadState())
                ;
            else
                hmiTaskSetState(HMI_STATE_POWER_ON);
            break;
        case Q_EXIT_SIG:
            break;
        case Q_HANDLE_SIG:
            break;
        case Q_FUNCTION_TIME_OUT_SIG:
            break;
        case Q_WORK_TIME_OUT_SIG:
            if (BATTERY_STATE_LOW_SYSTEM_LOCK == batteryTaskReadState())
            {
                state = Q_TRAN(lock_fsm_low_power_system_lock);
            }
            else if (Enabled == readUserParameter(USER_PARA_BREAK_ID))
            {
                keyTaskHandle(KEY_TYPE_TAMPER_KEY, false);
                hmi_task_tamper_warn_time(TAMPER_WARN_KEEP_TIME);
            }
            state = Q_TRAN(lock_fsm_idle);
            break;
        default:
            break;
    }

    return state;
}

QState lock_fsm_init(LockFsm *me, QEvent const *e)
{
    (void)e;
    
    OB_LOGD(TAG, "Fsm_state[%s], Event[%u, %u]", "init", e->sig, e->dynamic_[0]);

    return Q_TRAN(lock_fsm_power_on);
}
