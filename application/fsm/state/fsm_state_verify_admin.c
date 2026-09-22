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

#define OB_LOG_LEVEL OB_LOG_LEVEL_DEFAULT
#include "ob_log.h"
#define TAG "fsm_verify_admin"

/***************Variable***************/


/***************Function***************/


// ------------------------------------------

QState lock_fsm_verify_admin(LockFsm *me, QEvent const *e)
{
    QState state = Q_IGNORED();     //  没有对应事件就返回Q_IGNORED()

    OB_LOGD(TAG, "Fsm_state[%s], Event[%u, %u]", "verify_admin", e->sig, e->dynamic_[0]);

    switch (e->sig){
        case Q_ENTRY_SIG:
            keyEventInit();
            keyTaskHandle(KEY_TYPE_KEY_BOARD, true);           //key board
            fp_task_set_mode(FP_MODE_VERIFY);
            nfc_task_set_state(NFC_STATE_VERIFY);
            hmiTaskSetAllowSelintFlag(false);
            system_time_task_set_work_time(WORK_TIME_OUT_VAULE);
            hmiTaskSetState(HMI_STATE_VERIFY_ADMIN_CODE);
            break;
        case Q_EXIT_SIG:
            break;
        case Q_KEY_BOARD_PRESS_SIG:
            system_time_task_set_work_time(WORK_TIME_OUT_VAULE);
            if (KEY_NUM_13 == e->dynamic_[0])   // 门铃
            {
                hmiTaskSetState(HMI_STATE_BELL);
                uart_msg_bell(2, 1000);
            }
            else
            {
                hmiTaskSetState(HMI_STATE_KEY_BOARD_PRESS);
                keyEventVerifyAdmin(e->dynamic_[0]);
            }
            break;
        case Q_HANDLE_SIG:
            if (EVENT_RESULT_VOICE_TIME_OUT == e->dynamic_[0])
            {
                system_time_task_set_work_time(WORK_TIME_OUT_VAULE);
            }
            else if (EVENT_RESULT_VERIFY_INPUT_ERROR == e->dynamic_[0])
            {
                me->branch = (QStateHandler)(lock_fsm_idle);
                state = Q_TRAN(lockFsmInputError);
            }
            else if ((EVENT_RESULT_FAIL_TOO_SHORT == e->dynamic_[0]) || (EVENT_RESULT_FAIL_TOO_LONG == e->dynamic_[0]))
            {
                me->branch = (QStateHandler)(lock_fsm_idle);
                state = Q_TRAN(lockFsmVerifyFail);
            }
            else if (EVENT_RESULT_FAIL == e->dynamic_[0])
            {
                state = Q_TRAN(lock_fsm_idle); // 输入密码空的时候,再按*键
            }
            break;
        case Q_USER_HANDLE_SIG:
            // me->branch = (QStateHandler)(lock_fsm_verify_admin);

            if (EVENT_RESULT_SUCCESS_VERIFY_ADMIN == (e->dynamic_[0]))
            {
                if (isEmptyUser(false))   // 初始化状态
                {
                    hmiTaskSetState(HMI_STATE_PIN_CODE_TOO_SIMPLE);
                    state = Q_TRAN(lock_fsm_menu_modfiy_admin_pin);
                }
                else
                    state = Q_TRAN(lock_fsm_menu_admin);
            }
            else if (EVENT_RESULT_FAIL_INVALID == (e->dynamic_[0]))
            {
                me->branch = (QStateHandler)(lock_fsm_verify_admin);
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
