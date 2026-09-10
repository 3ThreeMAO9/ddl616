#include "fsm_state.h"
#include "state_inside.h"

#include "event.h"
#include "key_event.h"
#include "parameter.h"

#include "task_sleep.h"
#include "task_system_time.h"
#include "task_hmi.h"
#include "task_face.h"
#include "task_fingerprint.h"
#include "task_motor.h"
#include "task_key.h"
#include "task_nfc.h"

#define OB_LOG_LEVEL OB_LOG_LEVEL_DEFAULT
#include "ob_log.h"
#define TAG "fsm_idle"

/***************Variable***************/


/***************Function***************/


// ------------------------------------------

QState lock_fsm_idle(LockFsm *me, QEvent const *e)
{
    QState state = Q_IGNORED();     //  没有对应事件就返回Q_IGNORED()

    OB_LOGD(TAG, "Fsm_state[%s], Event[%u, %u]", "idle", e->sig, e->dynamic_[0]);

    switch (e->sig){
        case Q_ENTRY_SIG:
            keyTaskInit(KEY_TYPE_KEY_BOARD);
            keyTaskHandle(KEY_TYPE_KEY_BOARD, true);           //key board
            fp_task_set_mode(FP_MODE_VERIFY);
            nfc_task_set_state(NFC_STATE_VERIFY);
            hmiTaskSetAllowSelintFlag(true);
            // face_task_set_mode(FACE_TASK_MODE_DEFAULT);
            system_time_task_set_work_time(WORK_TIME_OUT_VAULE);
            hmiTaskSetState(HMI_STATE_KEY_BOARD_LED_ON);
            break;
        case Q_EXIT_SIG:
            break;
        case Q_KEY_BOARD_PRESS_SIG:
            if (KEY_NUM_13 == e->dynamic_[0])   // 门铃
            {
                hmiTaskSetState(HMI_STATE_ENTER_ADMIN_MODE);
                state = Q_TRAN(lock_fsm_verify_admin);
            }
            else
            {
                keyEventVerifyUser(e->dynamic_[0]);
                hmiTaskSetState(HMI_STATE_KEY_BOARD_PRESS);
            }
            break;
        case Q_KEY_PRESS_SIG:
            break;
        case Q_HANDLE_SIG:
            if ((EVENT_RESULT_FAIL_TOO_SHORT == e->dynamic_[0]) || (EVENT_RESULT_FAIL_TOO_LONG == e->dynamic_[0]))
            {
                me->branch = (QStateHandler)(lock_fsm_idle);
                state = Q_TRAN(lockFsmInputError);
            }
            else if (EVENT_RESULT_VOICE_MODE == e->dynamic_[0])
            {
                keyEventInit(); // 清空已输入的密码
                if (readUserParameter(USER_PARA_SILENT_MODE_ID)){
                    setUserParameter(USER_PARA_SILENT_MODE_ID, Disabled);
                }
                else{
                    setUserParameter(USER_PARA_SILENT_MODE_ID, Enabled);
                }
                me->branch = (QStateHandler)(lock_fsm_idle);
                state = Q_TRAN(lockFsmHandleVoiceModeSuccess);
            }
            // if (e->dynamic_[0] == HANDLE_EVENT_UART_RX){
            //     system_time_task_set_work_time(WORK_TIME_OUT_VAULE);
            // }
            // else if (e->dynamic_[0] == HANDLE_EVENT_SLEEP){
            //     state = Q_TRAN(lock_fsm_sleep);
            // }
            break;
        case Q_USER_KEY_SIG:
            break;
        case Q_USER_HANDLE_SIG:
            me->branch = (QStateHandler)(lock_fsm_idle);

            if (EVENT_RESULT_SUCCESS_VERIFY_USER == (e->dynamic_[0]))
            {
                motorTaskHandle(MOTOR_HANDLE_UNLOCK, 0);
                me->branch = (QStateHandler)(lock_fsm_sleep);
                state = Q_TRAN(lockFsmVerifyUserSuccess);
            }
            else if (EVENT_RESULT_FAIL_INVALID == (e->dynamic_[0]))
            {
                me->branch = (QStateHandler)(lock_fsm_idle);
                state = Q_TRAN(lockFsmVerifyFail);
            }
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
