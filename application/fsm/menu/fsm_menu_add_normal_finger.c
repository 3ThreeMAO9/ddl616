#include "fsm_state.h"
#include "state_inside.h"
#include "task_hmi.h"
#include "task_system_time.h"
#include "task_fingerprint.h"
#include "task_key.h"
#include "task_nfc.h"

#include "user.h"
#include "event.h"
#include "key_event.h"

#define OB_LOG_LEVEL OB_LOG_LEVEL_DEFAULT
#include "ob_log.h"
#define TAG "fsm_add_normal_finger"

/***************Variable***************/


/***************Function***************/
QState lock_fsm_menu_add_normal_finger(LockFsm *me, QEvent const *e)
{
    QState state = Q_IGNORED();     //  没有对应事件就返回Q_IGNORED()


    switch (e->sig){
        case Q_ENTRY_SIG:
            keyEventInit();
            keyTaskHandle(KEY_TYPE_KEY_BOARD, true);           //key board
            fp_task_set_mode(FP_MODE_REGISTER);
            nfc_task_set_state(NFC_STATE_SLEEP);
            system_time_task_set_work_time(WORK_TIME_OUT_VAULE);
            hmiTaskSetState(HMI_STATE_ADD_NORMAL_FINGER);
            break;
        case Q_EXIT_SIG:
            break;
        case Q_KEY_BOARD_PRESS_SIG:
            if (KEY_CAN == e->dynamic_[0])
            {
                hmiTaskSetState(HMI_STATE_KEY_BOARD_PRESS);
                state = Q_TRAN(lock_fsm_menu_add_normal_user_settings);
            }
            break;
        case Q_HANDLE_SIG:
            if (EVENT_RESULT_VOICE_TIME_OUT == e->dynamic_[0])
                system_time_task_set_work_time(WORK_TIME_OUT_VAULE);
            break;
        case Q_USER_HANDLE_SIG:
            if (EVENT_RESULT_SUCCESS_ADD == e->dynamic_[0])
            {
                me->branch = (QStateHandler)(lock_fsm_menu_add_normal_user_settings);
                state = Q_TRAN(lockFsmHandleAddSuccess);
            }
            else if (EVENT_RESULT_FINGERPRINT_PRESS == e->dynamic_[0])
            {
                hmiTaskSetState(HMI_STATE_ENROLL_FINGER_PRESS);
            }
            else if(EVENT_RESULT_FAIL_ADD == e->dynamic_[0])
            {
                me->branch = (QStateHandler)(lock_fsm_menu_add_normal_user_settings);
                state = Q_TRAN(lockFsmHandleAddFail);
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
