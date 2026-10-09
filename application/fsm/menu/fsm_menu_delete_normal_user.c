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
#define TAG "fsm_delete_normal_user"

// 流程（对应语音表）：
//   删除用户菜单    : 删除单个用户请按1，删除全部用户请按2，返回上级菜单请按星号键
//   输入用户编号    : 请输入要删除的用户编号，以#号键结束，返回上级菜单请按星号键
//   确认删除全部    : 删除全部普通用户，以#号键结束，返回上级菜单请按星号键
//   结果            : 删除成功 / 删除失败+管理用户不可删除 / 删除失败+用户不存在 /
//                     删除失败（用户为空）/ 删除失败+操作超时   —— 播完回到删除用户菜单

/***************Variable***************/
#define DELETE_ID_INPUT_MAX     (4)         // 用户档案ID 最大 49，4 位够用

static uint8_t  delete_id_len = 0;
static uint8_t  delete_id_buf[DELETE_ID_INPUT_MAX] = {0};

/***************Function***************/
static void delete_id_input_clear(void)
{
    delete_id_len = 0;
    memset(delete_id_buf, 0, sizeof(delete_id_buf));
}

static uint16_t delete_id_input_value(void)
{
    uint16_t value = 0;
    uint8_t  i;

    for (i = 0; i < delete_id_len; i++)
    {
        value = value * 10 + delete_id_buf[i];
    }

    return value;
}

// 删除结果 -> 对应结果语音；语音播完由 lockFsmSuccessDeal 回到 me->branch（删除用户菜单）
static QState delete_result_deal(LockFsm *me, uint8_t result)
{
    me->branch = (QStateHandler)(lock_fsm_menu_delete_normal_user);

    switch (result)
    {
    case USER_DEL_OK:
        return Q_TRAN(lockFsmDeleteUserSuccess);
    case USER_DEL_FAIL_ADMIN:
        return Q_TRAN(lockFsmDeleteUserFailAdmin);
    case USER_DEL_FAIL_NOT_EXIST:
        return Q_TRAN(lockFsmDeleteUserFailNotExist);
    case USER_DEL_FAIL_EMPTY:
        return Q_TRAN(lockFsmDeleteUserFailEmpty);
    default:                            // 超时未输入
        return Q_TRAN(lockFsmDeleteUserFailTimeOut);
    }
}

// ------------------------------------------
// 删除普通用户菜单：1=删除单个，2=删除全部，星号键返回管理员菜单
QState lock_fsm_menu_delete_normal_user(LockFsm *me, QEvent const *e)
{
    QState state = Q_IGNORED();     //  没有对应事件就返回Q_IGNORED()

    OB_LOGD(TAG, "Fsm_state[%s], Event[%u, %u]", "delete_normal_user", e->sig, e->dynamic_[0]);

    switch (e->sig){
        case Q_ENTRY_SIG:
            delete_id_input_clear();
            keyEventInit();
            keyTaskHandle(KEY_TYPE_KEY_BOARD, true);            //key board
            fp_task_set_mode(FP_MODE_IDLE);
            nfc_task_set_state(NFC_STATE_SLEEP);
            system_time_task_set_work_time(WORK_TIME_OUT_VAULE);
            hmiTaskSetState(HMI_STATE_DELETE_NORMAL_USER);
            break;
        case Q_EXIT_SIG:
            break;
        case Q_KEY_BOARD_PRESS_SIG:
            system_time_task_set_work_time(WORK_TIME_OUT_VAULE);
            hmiTaskSetState(HMI_STATE_KEY_BOARD_PRESS);
            if (KEY_NUM_1 == e->dynamic_[0])
            {
                state = Q_TRAN(lock_fsm_menu_delete_normal_user_input_id);  // 删除单个用户
            }
            else if (KEY_NUM_2 == e->dynamic_[0])
            {
                state = Q_TRAN(lock_fsm_menu_delete_all_normal_user);       // 删除全部普通用户
            }
            else if (KEY_CAN == e->dynamic_[0])                             // 星号键：返回管理员菜单
            {
                state = Q_TRAN(lock_fsm_menu_admin);
            }
            break;
        case Q_HANDLE_SIG:
            if (EVENT_RESULT_VOICE_TIME_OUT == e->dynamic_[0])
                system_time_task_set_work_time(WORK_TIME_OUT_VAULE);
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
// 输入要删除的用户编号：数字 + #号键结束，星号键返回
QState lock_fsm_menu_delete_normal_user_input_id(LockFsm *me, QEvent const *e)
{
    QState  state = Q_IGNORED();
    uint8_t key = (uint8_t)e->dynamic_[0];

    OB_LOGD(TAG, "Fsm_state[%s], Event[%u, %u]", "delete_normal_user_input_id", e->sig, key);

    switch (e->sig){
        case Q_ENTRY_SIG:
            delete_id_input_clear();
            keyTaskHandle(KEY_TYPE_KEY_BOARD, true);            //key board
            fp_task_set_mode(FP_MODE_IDLE);
            nfc_task_set_state(NFC_STATE_SLEEP);
            system_time_task_set_work_time(WORK_TIME_OUT_VAULE);
            hmiTaskSetState(HMI_STATE_DELETE_NORMAL_USER_INPUT_ID);
            break;
        case Q_EXIT_SIG:
            break;
        case Q_KEY_BOARD_PRESS_SIG:
            system_time_task_set_work_time(WORK_TIME_OUT_VAULE);
            if (key <= KEY_NUM_9)                               // 数字：攒进缓冲
            {
                hmiTaskSetState(HMI_STATE_KEY_BOARD_PRESS);
                if (delete_id_len < DELETE_ID_INPUT_MAX)
                {
                    delete_id_buf[delete_id_len++] = key;
                }
            }
            else if (KEY_CAN == key)                            // 星号键：返回删除用户菜单
            {
                hmiTaskSetState(HMI_STATE_KEY_BOARD_PRESS);
                state = Q_TRAN(lock_fsm_menu_delete_normal_user);
            }
            else if (KEY_OK == key)                             // 井号键：按输入的编号删除
            {
                state = delete_result_deal(me, (0 == delete_id_len)
                                                ? USER_DEL_FAIL_NOT_EXIST
                                                : user_delete_normal(delete_id_input_value()));
            }
            break;
        case Q_HANDLE_SIG:
            if (EVENT_RESULT_VOICE_TIME_OUT == e->dynamic_[0])
                system_time_task_set_work_time(WORK_TIME_OUT_VAULE);
            break;
        case Q_WORK_TIME_OUT_SIG:                               // 超时未输入
            state = delete_result_deal(me, 0xFF);
            break;
        default:
            break;
    }

    return state;
}

// ------------------------------------------
// 删除全部普通用户：以 #号键确认，星号键返回
QState lock_fsm_menu_delete_all_normal_user(LockFsm *me, QEvent const *e)
{
    QState  state = Q_IGNORED();
    uint8_t key = (uint8_t)e->dynamic_[0];

    OB_LOGD(TAG, "Fsm_state[%s], Event[%u, %u]", "delete_all_normal_user", e->sig, key);

    switch (e->sig){
        case Q_ENTRY_SIG:
            keyEventInit();
            keyTaskHandle(KEY_TYPE_KEY_BOARD, true);            //key board
            fp_task_set_mode(FP_MODE_IDLE);
            nfc_task_set_state(NFC_STATE_SLEEP);
            system_time_task_set_work_time(WORK_TIME_OUT_VAULE);
            hmiTaskSetState(HMI_STATE_DELETE_ALL_NORMAL_USER_CONFIRM);
            break;
        case Q_EXIT_SIG:
            break;
        case Q_KEY_BOARD_PRESS_SIG:
            system_time_task_set_work_time(WORK_TIME_OUT_VAULE);
            if (KEY_CAN == key)                                 // 星号键：返回删除用户菜单
            {
                hmiTaskSetState(HMI_STATE_KEY_BOARD_PRESS);
                state = Q_TRAN(lock_fsm_menu_delete_normal_user);
            }
            else if (KEY_OK == key)                             // 井号键：确认删除全部普通用户
            {
                state = delete_result_deal(me, user_delete_all_normal());
            }
            else
            {
                hmiTaskSetState(HMI_STATE_KEY_BOARD_PRESS);
            }
            break;
        case Q_HANDLE_SIG:
            if (EVENT_RESULT_VOICE_TIME_OUT == e->dynamic_[0])
                system_time_task_set_work_time(WORK_TIME_OUT_VAULE);
            break;
        case Q_WORK_TIME_OUT_SIG:                               // 超时未输入
            state = delete_result_deal(me, 0xFF);
            break;
        default:
            break;
    }

    return state;
}
