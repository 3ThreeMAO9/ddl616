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
#define TAG "fsm_add_normal_user_settings"

/***************Variable***************/


/***************Function***************/
static QState menu_event_handle(LockFsm *me, QEvent const *e)
{
    QState state = Q_IGNORED(); //  没有对应事件就返回Q_IGNORED()

#if (Enabled == PRINTF_FSM)
    OB_LOGD(TAG, "--Now State[user_setting], Event[%d, %d]--", e->sig, e->dynamic_[0]);
#endif
    switch (e->dynamic_[0])
    {
    case KEY_NUM_1:
        hmiTaskSetState(HMI_STATE_KEY_BOARD_PRESS);
        state = Q_TRAN(lock_fsm_menu_add_normal_pw);
        break;
    case KEY_NUM_2:
        hmiTaskSetState(HMI_STATE_KEY_BOARD_PRESS);
        state = Q_TRAN(lock_fsm_menu_add_normal_finger);
        break;
    case KEY_NUM_3:
        hmiTaskSetState(HMI_STATE_KEY_BOARD_PRESS);
        state = Q_TRAN(lock_fsm_menu_add_normal_nfc);
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
QState lock_fsm_menu_add_normal_user_settings(LockFsm *me, QEvent const *e)
{
    QState state = Q_IGNORED();     //  没有对应事件就返回Q_IGNORED()

    OB_LOGD(TAG, "Fsm_state[%s], Event[%u, %u]", "add_normal_user_settings", e->sig, e->dynamic_[0]);

    switch (e->sig){
        case Q_ENTRY_SIG:
            // 档案ID 在"添加普通用户"菜单入口（lock_fsm_menu_admin 的 KEY_NUM_1）已经分配好，
            // 这里不能再分配：从密码/指纹/卡片子菜单加完返回本菜单会重入 ENTRY，
            // 再分一次会让同一个用户的第二把钥匙落到下一个档案ID 上
            keyEventInit();
            keyTaskHandle(KEY_TYPE_KEY_BOARD, true);           //key board
            fp_task_set_mode(FP_MODE_IDLE);
            nfc_task_set_state(NFC_STATE_SLEEP);
            system_time_task_set_work_time(WORK_TIME_OUT_VAULE);
            hmiTaskSetState(HMI_STATE_ADD_NORMAL_USER);
            break;
        case Q_EXIT_SIG:
            break;
        case Q_KEY_BOARD_PRESS_SIG:
            system_time_task_set_work_time(WORK_TIME_OUT_VAULE);
            state = menu_event_handle(me, e);
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
