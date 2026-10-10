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
#define TAG "fsm_modify_admin_user_settings"

/***************Variable***************/


/***************Function***************/
static QState lock_fsm_menu_modfiy_admin_finger(LockFsm *me, QEvent const *e);   // 添加/修改管理指纹
static QState menu_event_handle(LockFsm *me, QEvent const *e)
{
    QState state = Q_IGNORED(); //  没有对应事件就返回Q_IGNORED()

    switch (e->dynamic_[0])
    {
    case KEY_NUM_1:
        hmiTaskSetState(HMI_STATE_KEY_BOARD_PRESS);
        state = Q_TRAN(lock_fsm_menu_modfiy_admin_pin);
        break;
    case KEY_NUM_2:
        hmiTaskSetState(HMI_STATE_KEY_BOARD_PRESS);
        state = Q_TRAN(lock_fsm_menu_modfiy_admin_finger);      // 修改管理指纹
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
QState lock_fsm_menu_modify_admin_user_settings(LockFsm *me, QEvent const *e)
{
    QState state = Q_IGNORED();     //  没有对应事件就返回Q_IGNORED()


    switch (e->sig){
        case Q_ENTRY_SIG:
            alloc_user_id();        // 管理员档案固定 0（见 modifyMasterKeyCode），这里分配的值实际未被使用
            keyEventInit();
            keyTaskHandle(KEY_TYPE_KEY_BOARD, true);           //key board
            fp_task_set_mode(FP_MODE_IDLE);
            nfc_task_set_state(NFC_STATE_SLEEP);
            system_time_task_set_work_time(WORK_TIME_OUT_VAULE);
            hmiTaskSetState(HMI_STATE_MODIFY_ADMIN_USER_SETTINGS);
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

// ------------------------------------------
// 添加/修改管理指纹：录入成功后由 task_fingerprint 存成管理指纹（key_id 0、归属管理员档案 0）
// 重复录入直接覆盖原来那把管理指纹，不占普通指纹编号
static QState lock_fsm_menu_modfiy_admin_finger(LockFsm *me, QEvent const *e)
{
    QState state = Q_IGNORED();     //  没有对应事件就返回Q_IGNORED()

    switch (e->sig){
        case Q_ENTRY_SIG:
            keyEventInit();
            keyTaskHandle(KEY_TYPE_KEY_BOARD, true);           //key board
            nfc_task_set_state(NFC_STATE_SLEEP);
            system_time_task_set_work_time(WORK_TIME_OUT_VAULE);
            fp_task_set_mode(FP_MODE_REGISTER_MASTER);      // 管理指纹：合并模板后直接覆盖模块 0 号区域
            hmiTaskSetState(HMI_STATE_ADD_NORMAL_FINGER);   // 请触摸指纹传感器，返回上级菜单请按*号键
            break;
        case Q_EXIT_SIG:
            break;
        case Q_KEY_BOARD_PRESS_SIG:
            if (KEY_CAN == e->dynamic_[0])
            {
                hmiTaskSetState(HMI_STATE_KEY_BOARD_PRESS);
                state = Q_TRAN(lock_fsm_menu_modify_admin_user_settings);
            }
            break;
        case Q_HANDLE_SIG:
            if (EVENT_RESULT_VOICE_TIME_OUT == e->dynamic_[0])
                system_time_task_set_work_time(WORK_TIME_OUT_VAULE);
            break;
        case Q_USER_HANDLE_SIG:
            if (EVENT_RESULT_SUCCESS_ADD == e->dynamic_[0])
            {
                me->branch = (QStateHandler)(lock_fsm_menu_modify_admin_user_settings);
                state = Q_TRAN(lockFsmHandleAddSuccess);
            }
            else if (EVENT_RESULT_FINGERPRINT_PRESS == e->dynamic_[0])
            {
                hmiTaskSetState(HMI_STATE_ENROLL_FINGER_PRESS);
            }
            else if (EVENT_RESULT_FAIL_ADD == e->dynamic_[0])
            {
                me->branch = (QStateHandler)(lock_fsm_menu_modify_admin_user_settings);
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
