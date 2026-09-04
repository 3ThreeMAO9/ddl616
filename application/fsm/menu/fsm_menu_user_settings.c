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
#define TAG "fsm_menu_user_settings"

/***************Variable***************/


/***************Function***************/
static QState menu_user_settings_event_handle(LockFsm *me, QEvent const *e)
{
    QState state = Q_IGNORED(); //  没有对应事件就返回Q_IGNORED()

    switch (e->dynamic_[0])
    {
    case KEY_NUM_1:
        hmiTaskSetState(HMI_STATE_KEY_BOARD_PRESS);
        state = Q_TRAN(lock_fsm_menu_modfiy_admin_pin);     // 修改管理用户
        break;
    case KEY_NUM_2:
        hmiTaskSetState(HMI_STATE_KEY_BOARD_PRESS);
        state = Q_TRAN(lock_fsm_menu_add_normal_user);      // 添加普通用户
        break;
    case KEY_CAN:
        hmiTaskSetState(HMI_STATE_KEY_BOARD_PRESS);
        state = Q_TRAN(lock_fsm_menu_admin);
        break;
    default:
        break;
    }
    return state;
}

// ------------------------------------------
QState lock_fsm_menu_user_settings(LockFsm *me, QEvent const *e)
{
    QState state = Q_IGNORED();     //  没有对应事件就返回Q_IGNORED()

    OB_LOGD(TAG, "Fsm_state[%s], Event[%u, %u]", "menu_user_settings", e->sig, e->dynamic_[0]);

    switch (e->sig){
        case Q_ENTRY_SIG:
            keyEventInit();
            keyTaskHandle(KEY_TYPE_KEY_BOARD, true);           //key board
            system_time_task_set_work_time(WORK_TIME_OUT_VAULE);
            hmiTaskSetState(HMI_STATE_USER_SETTINGS);
#if (Enabled == PRINTF_FSM)
            OB_LOGI(TAG, "case KEY_NUM_1:    //修改管理用户请按1");
            OB_LOGI(TAG, "case KEY_NUM_2:    //添加普通用户请按2");
#endif
            break;
        case Q_EXIT_SIG:
            break;
        case Q_KEY_BOARD_PRESS_SIG:
            state = menu_user_settings_event_handle(me, e);
            break;
        case Q_HANDLE_SIG:
            if (EVENT_RESULT_VOICE_TIME_OUT == e->dynamic_[0])
                system_time_task_set_work_time(WORK_TIME_OUT_VAULE);
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
