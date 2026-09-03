#include "fsm_state.h"
#include "state_inside.h"
#include "task_hmi.h"
#include "task_system_time.h"

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
            system_time_task_set_function_time(1200);
            system_time_task_set_work_time(1000);
            hmiTaskSetState(HMI_STATE_POWER_ON);
            break;
        case Q_EXIT_SIG:
            break;
        case Q_HANDLE_SIG:
            break;
        case Q_USER_KEY_SIG:
            break;
        case Q_FUNCTION_TIME_OUT_SIG:
            break;
        case Q_WORK_TIME_OUT_SIG:
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
