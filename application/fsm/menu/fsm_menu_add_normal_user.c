#include "fsm_state.h"
#include "state_inside.h"
#include "task_hmi.h"
#include "task_system_time.h"
#include "task_fingerprint.h"
#include "task_key.h"
#include "task_nfc.h"

#include "event.h"
#include "key_event.h"

#define OB_LOG_LEVEL OB_LOG_LEVEL_DEFAULT
#include "ob_log.h"
#define TAG "fsm_menu_add_normal_user"

/***************Variable***************/


/***************Function***************/
static QState lock_fsm_menu_repeat_add_code(LockFsm *me, QEvent const *e)
{
    return lock_fsm_menu_repeat_input_code(me, e, CODE_HANDLE_ADD);
}

// ------------------------------------------
QState lock_fsm_menu_add_normal_user(LockFsm *me, QEvent const *e)
{
    QState state = Q_IGNORED();     //  没有对应事件就返回Q_IGNORED()

    OB_LOGD(TAG, "Fsm_state[%s], Event[%u, %u]", "add_normal_user", e->sig, e->dynamic_[0]);

    switch (e->sig){
        case Q_ENTRY_SIG:
            keyEventInit();
            keyTaskHandle(KEY_TYPE_KEY_BOARD, true);           //key board
            fp_task_set_mode(FP_MODE_REGISTER);
            nfc_task_set_state(NFC_STATE_REGISTER);
            system_time_task_set_work_time(WORK_TIME_OUT_VAULE);
            hmiTaskSetState(HMI_STATE_ADD_NORMAL_USER);
            break;
        case Q_EXIT_SIG:
            break;
        case Q_KEY_BOARD_PRESS_SIG:
            if (KEY_CAN == e->dynamic_[0])
            {
                hmiTaskSetState(HMI_STATE_KEY_BOARD_PRESS);
                state = Q_TRAN(lock_fsm_menu_user_settings);
            }
            else
            {
                hmiTaskSetState(HMI_STATE_KEY_BOARD_PRESS);
                keyEventHandleCode((e->dynamic_[0]), CODE_HANDLE_ADD, 0);
            }
            break;
        case Q_HANDLE_SIG:
            if (EVENT_RESULT_VOICE_TIME_OUT == e->dynamic_[0])
                system_time_task_set_work_time(WORK_TIME_OUT_VAULE);
            else if (EVENT_RESULT_SUCCESS == e->dynamic_[0])
            {
                state = Q_TRAN(lock_fsm_menu_repeat_add_code);
            }
            else if (EVENT_RESULT_FAIL_INPUT == e->dynamic_[0])
            {
                hmiTaskSetState(HMI_STATE_INPUT_ERROR_AGAIN);
                state = Q_TRAN(lock_fsm_menu_add_normal_user);      // 添加普通用户
            }
            else if (EVENT_RESULT_FAIL == e->dynamic_[0])
            {
                OB_LOGD(TAG,"EVENT_RESULT_FAIL");
            }
            else if (EVENT_RESULT_FAIL_TOO_SIMPLE == e->dynamic_[0])
            {
                hmiTaskSetState(HMI_STATE_PIN_CODE_TOO_SIMPLE);
                state = Q_TRAN(lock_fsm_menu_add_normal_user);      // 添加普通用户
            }
            else if (EVENT_RESULT_FAIL_REPEAT == e->dynamic_[0])
            {
                hmiTaskSetState(HMI_STATE_PIN_REPEAT);
                state = Q_TRAN(lock_fsm_menu_add_normal_user);      // 添加普通用户
            }
            break;
        case Q_USER_HANDLE_SIG:
            if (EVENT_RESULT_SUCCESS_ADD == e->dynamic_[0])
            {
                me->branch = (QStateHandler)(lock_fsm_menu_add_normal_user);
                state = Q_TRAN(lockFsmHandleAddSuccess);
            }
            else if (EVENT_RESULT_FAIL_CARD_REPEAT == e->dynamic_[0])
            {
                me->branch = (QStateHandler)(lock_fsm_menu_add_normal_user);
                state = Q_TRAN(lockFsmHandleCardRepeat);
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
