#include "fsm_state.h"
#include "state_inside.h"

#include "task_sleep.h"
#include "task_system_time.h"
#include "event.h"
#include "task_led.h"
#include "task_face.h"
#include "task_radar.h"
#include "task_fingerprint.h"


#define OB_LOG_LEVEL OB_LOG_LEVEL_DEFAULT
#include "ob_log.h"
#define TAG "lock_fsm"

/***************Variable***************/


/***************Function***************/


// ------------------------------------------

QState lock_fsm_idle(LockFsm *me, QEvent const *e)
{
    QState state = Q_IGNORED();     //  没有对应事件就返回Q_IGNORED()

    OB_LOGD(TAG, "Fsm_state[%s], Event[%u, %u]", "idle", e->sig, e->dynamic_[0]);

    switch (e->sig){
        case Q_ENTRY_SIG:
            fp_task_set_mode(FP_TASK_MODE_DEFAULT);
            face_task_set_mode(FACE_TASK_MODE_DEFAULT);
            system_time_task_set_work_time(WORK_TIME_OUT_VAULE);
            break;
        case Q_EXIT_SIG:
            break;
        case Q_KEY_BOARD_PRESS_SIG:
            break;
        case Q_KEY_PRESS_SIG:
            break;
        case Q_HANDLE_SIG:
            if (e->dynamic_[0] == HANDLE_EVENT_UART_RX){
                system_time_task_set_work_time(WORK_TIME_OUT_VAULE);
            }
            break;
        case Q_USER_KEY_SIG:
            break;
        case Q_FUNCTION_TIME_OUT_SIG:
            break;
        case Q_WORK_TIME_OUT_SIG:
            state = Q_TRAN(lock_fsm_sleep);
            break;
        default:
            break;
    }

    return state;
}
