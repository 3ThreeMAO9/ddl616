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
#define TAG "fsm_repeat_input_code"

/***************Variable***************/


/***************Function***************/


// ------------------------------------------
QState lock_fsm_menu_repeat_input_code(LockFsm *me, QEvent const *e, uint8_t code_handle)
{
    QState state = Q_IGNORED();     //  没有对应事件就返回Q_IGNORED()


    switch (e->sig){
        case Q_ENTRY_SIG:
            keyTaskHandle(KEY_TYPE_KEY_BOARD, true);           //key board
            fp_task_set_mode(FP_MODE_IDLE);
            nfc_task_set_state(NFC_STATE_SLEEP);
            system_time_task_set_function_time(MENU_TIME_OUT_VAULE);
            hmiTaskSetState(HMI_STATE_REPEAT_INPUT_CODE);
            break;
        case Q_EXIT_SIG:
            break;
        case Q_KEY_BOARD_PRESS_SIG:
            if (e->dynamic_[0] <= KEY_OK)
            {
                hmiTaskSetState(HMI_STATE_KEY_BOARD_PRESS);
                system_time_task_set_function_time(MENU_TIME_OUT_VAULE);
                keyEventHandleCode((e->dynamic_[0]), code_handle, 1);
            }
            break;
        case Q_HANDLE_SIG:
            if (EVENT_RESULT_VOICE_TIME_OUT == e->dynamic_[0])
                system_time_task_set_function_time(MENU_TIME_OUT_VAULE);
            else if ((EVENT_RESULT_CANCEL_ADD_KEY == e->dynamic_[0])
                     && (CODE_HANDLE_ADD == code_handle))    // 云端(0x03)取消：只对"添加普通用户"这条流程生效
            {
                state = Q_TRAN(lock_fsm_menu_add_normal_user_settings);
            }
            else if (EVENT_RESULT_SUCCESS == e->dynamic_[0])
            {
                if (CODE_HANDLE_ADD == e->dynamic_[1])
                {
                    me->branch = (QStateHandler)(lock_fsm_menu_add_normal_user_settings);
                }
                else if (CODE_HANDLE_CHANGE_MASTER == e->dynamic_[1])
                {
                    if (me->admin_flag == false)
                        me->branch = (QStateHandler)(lock_fsm_menu_admin);
                    else
                        me->branch = (QStateHandler)(lock_fsm_menu_modify_admin_user_settings);
                }
                else if (CODE_HANDLE_DEL == e->dynamic_[1])
                {

                }
                state = Q_TRAN(lockFsmHandleAddSuccess);
            }
            else if ((EVENT_RESULT_FAIL_DIFFERENT == e->dynamic_[0])
                    || (EVENT_RESULT_FAIL_INPUT == e->dynamic_[0])
                    ||  (EVENT_RESULT_FAIL_TOO_SIMPLE == e->dynamic_[0]))
            {
                if (EVENT_RESULT_FAIL_INPUT == e->dynamic_[0])
                    hmiTaskSetState(HMI_STATE_INPUT_ERROR_AGAIN);
                else
                    hmiTaskSetState(HMI_STATE_PIN_DIFFERENT);
                if (code_handle == CODE_HANDLE_CHANGE_MASTER)
                {
                    if (isEmptyKey(false))
                        state = Q_TRAN(lock_fsm_idle);
                    else
                        state = Q_TRAN(lock_fsm_menu_modfiy_admin_pin); // 修改管理用户
                }
                else if (code_handle == CODE_HANDLE_ADD)
                    state = Q_TRAN(lock_fsm_menu_add_normal_pw); // 添加普通用户
            }
            break;
        case Q_USER_HANDLE_SIG:
            break;
        case Q_WORK_TIME_OUT_SIG:                               // 系统工时到期：菜单里不做动作（停留超时由功能超时管）
            break;
        case Q_FUNCTION_TIME_OUT_SIG:       // 超时：播"添加失败，操作超时"后回上级菜单（本状态被多处复用，按来源区分）
            if (CODE_HANDLE_ADD == code_handle)
                me->branch = (QStateHandler)(lock_fsm_menu_add_normal_user_settings);
            else if (me->admin_flag == false)
                me->branch = (QStateHandler)(lock_fsm_menu_admin);
            else
                me->branch = (QStateHandler)(lock_fsm_menu_modify_admin_user_settings);
            state = Q_TRAN(lockFsmAddFailTimeOut);
            break;
        default:
            break;
    }

    return state;
}
