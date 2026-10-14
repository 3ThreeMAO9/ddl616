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
#define TAG "fsm_add_normal_nfc"

/***************Variable***************/


/***************Function***************/
QState lock_fsm_menu_add_normal_nfc(LockFsm *me, QEvent const *e)
{
    QState state = Q_IGNORED();     //  没有对应事件就返回Q_IGNORED()


    switch (e->sig){
        case Q_ENTRY_SIG:
            keyEventInit();
            keyTaskHandle(KEY_TYPE_KEY_BOARD, true);           //key board
            fp_task_set_mode(FP_MODE_IDLE);
            nfc_task_set_state(NFC_STATE_REGISTER);
            system_time_task_set_function_time(MENU_TIME_OUT_VAULE);
            hmiTaskSetState(HMI_STATE_ADD_NORMAL_NFC);
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
                system_time_task_set_function_time(MENU_TIME_OUT_VAULE);
            else if (EVENT_RESULT_CANCEL_ADD_KEY == e->dynamic_[0])     // 云端(0x03)取消添加：退出录入，回上级
                state = Q_TRAN(lock_fsm_menu_add_normal_user_settings);
            break;
        case Q_USER_HANDLE_SIG:
            if (EVENT_RESULT_SUCCESS_ADD == e->dynamic_[0])
            {
                me->branch = (QStateHandler)(lock_fsm_menu_add_normal_user_settings);
                state = Q_TRAN(lockFsmHandleAddSuccess);
            }
            else if (EVENT_RESULT_FAIL_CARD_REPEAT == e->dynamic_[0])
            {
                me->branch = (QStateHandler)(lock_fsm_menu_add_normal_user_settings);
                state = Q_TRAN(lockFsmHandleCardRepeat);
            }
            break;
        case Q_WORK_TIME_OUT_SIG:                               // 系统工时到期：菜单里不做动作（停留超时由功能超时管）
            break;
        case Q_FUNCTION_TIME_OUT_SIG:                               // 超时：播"添加失败，操作超时"后回上级（添加普通用户设置）
            me->branch = (QStateHandler)(lock_fsm_menu_add_normal_user_settings);
            state = Q_TRAN(lockFsmAddFailTimeOut);
            break;
        default:
            break;
    }

    return state;
}
