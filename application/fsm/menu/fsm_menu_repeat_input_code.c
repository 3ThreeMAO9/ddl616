#include "fsm_state.h"
#include "state_inside.h"
#include "task_hmi.h"
#include "task_system_time.h"
#include "task_fingerprint.h"
#include "task_key.h"

#include "event.h"
#include "key_event.h"

#define OB_LOG_LEVEL OB_LOG_LEVEL_DEFAULT
#include "ob_log.h"
#define TAG "fsm_menu_repeat_input_code"

/***************Variable***************/


/***************Function***************/


// ------------------------------------------
QState lock_fsm_menu_repeat_input_code(LockFsm *me, QEvent const *e, uint8_t code_handle)
{
    QState state = Q_IGNORED();     //  没有对应事件就返回Q_IGNORED()

    OB_LOGD(TAG, "Fsm_state[%s], Event[%u, %u]", "repeat_input_code", e->sig, e->dynamic_[0]);

    switch (e->sig){
        case Q_ENTRY_SIG:
            keyTaskHandle(KEY_TYPE_KEY_BOARD, true);           //key board
            system_time_task_set_work_time(WORK_TIME_OUT_VAULE);
            hmiTaskSetState(HMI_STATE_REPEAT_INPUT_CODE);
            break;
        case Q_EXIT_SIG:
            break;
        case Q_KEY_BOARD_PRESS_SIG:
            if (e->dynamic_[0] <= KEY_OK)
            {
                hmiTaskSetState(HMI_STATE_KEY_BOARD_PRESS);
                system_time_task_set_work_time(WORK_TIME_OUT_VAULE);
                keyEventHandleCode((e->dynamic_[0]), code_handle, 1);
            }
            break;
        case Q_HANDLE_SIG:
            if (EVENT_RESULT_VOICE_TIME_OUT == e->dynamic_[0])
                system_time_task_set_work_time(WORK_TIME_OUT_VAULE);
            else if (EVENT_RESULT_SUCCESS == e->dynamic_[0])
            {
                me->branch = (QStateHandler)(lock_fsm_menu_user_settings);
                state = Q_TRAN(lockFsmHandleAddSuccess);
            }
            else if ((EVENT_RESULT_FAIL_DIFFERENT == e->dynamic_[0])
                    || (EVENT_RESULT_FAIL_INPUT == e->dynamic_[0])
                    ||  (EVENT_RESULT_FAIL_TOO_SIMPLE == e->dynamic_[0]))
            {
                hmiTaskSetState(HMI_STATE_PIN_DIFFERENT);
                state = Q_TRAN(lock_fsm_menu_modfiy_admin_pin);     // 修改管理用户
            }
            break;
        case Q_USER_HANDLE_SIG:
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
