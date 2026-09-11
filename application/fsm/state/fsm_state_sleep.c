#include "fsm_state.h"
#include "state_inside.h"
#include "msg_protocol.h"

#include "task_sleep.h"
#include "task_system_time.h"

#include "task_fingerprint.h"
#include "task_hmi.h"
#include "task_key.h"
#include "task_nfc.h"

#include "event.h"
#include "key_event.h"

#define OB_LOG_LEVEL OB_LOG_LEVEL_DEBUG
#include "ob_log.h"
#define TAG "fsm_sleep"

/***************Variable***************/


/***************Function***************/

void lock_wake(void)
{
    uart_msg_wake(0x80);
}
// ------------------------------------------

QState lock_fsm_sleep(LockFsm *me, QEvent const *e){
    QState state = Q_IGNORED();     //  没有对应事件就返回Q_IGNORED()

    OB_LOGD(TAG, "Fsm_state[%s], Event[%u, %u]", "sleep", e->sig, e->dynamic_[0]);

    switch (e->sig){
        case Q_ENTRY_SIG:
            keyEventInit();
            keyTaskHandle(KEY_TYPE_KEY_BOARD, false);           //key board
            fp_task_set_mode(FP_MODE_SLEEP);
            nfc_task_set_state(NFC_STATE_SLEEP);
            hmiTaskSetAllowSelintFlag(true);
            system_time_task_set_work_time(250);
            hmiTaskSetState(HMI_STATE_KEY_BOARD_LED_OFF);
            break;
        case Q_EXIT_SIG:
            break;
        case Q_KEY_BOARD_PRESS_SIG:
            break;
        case Q_KEY_PRESS_SIG:
            break;
        case Q_HANDLE_SIG:
            if (HANDLE_EVENT_WAKE == e->dynamic_[0]){
                OB_LOGW(TAG,"wake [%08X]",e->dynamic_[1]);
                // lock_wake();
                if (e->dynamic_[1] == WAKE_UP_TYPE_KEY_BOARD)
                    hmiTaskSetState(HMI_STATE_KEY_BOARD_WAKE_UP);
                state = Q_TRAN(lock_fsm_idle);
            }
            else if (e->dynamic_[0] == HANDLE_EVENT_SLEEP_BUSY){
                system_time_task_set_work_time(250);
                OB_LOGI(TAG, "Fsm_state[%s], HANDLE_EVENT_SLEEP_BUSY!!!!", "sleep");
            }
            break;
        case Q_USER_KEY_SIG:
            break;
        case Q_FUNCTION_TIME_OUT_SIG:
            break;
        case Q_WORK_TIME_OUT_SIG:
            // if (fp_task_is_busy()) {
            //     system_time_task_set_work_time(250);
            //     break;
            // }
            sleep_task_set_flag();
            break;
        default:
            break;
    }

    return state;
}
