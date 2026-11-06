#include "fsm_state.h"
#include "state_inside.h"
#include "task_hmi.h"
#include "task_system_time.h"
#include "task_fingerprint.h"
#include "task_key.h"
#include "task_nfc.h"

#include "event.h"
#include "key_event.h"
#include "uart_protocol.h"

#define OB_LOG_LEVEL OB_LOG_LEVEL_DEFAULT
#include "ob_log.h"
#define TAG "fsm_exit_linked_unlock"

/***************Variable***************/
QState lock_fsm_menu_exit_linked_unlock(LockFsm *me, QEvent const *e)
{
    QState state = Q_IGNORED();     //  没有对应事件就返回Q_IGNORED()

    switch (e->sig){
        case Q_ENTRY_SIG:
            keyEventInit();
            keyTaskHandle(KEY_TYPE_KEY_BOARD, true);           //key board
            fp_task_set_mode(FP_MODE_IDLE);
            nfc_task_set_state(NFC_STATE_SLEEP);
            system_time_task_set_function_time(MENU_TIME_OUT_VAULE);
            hmiTaskSetState(HMI_STATE_EXIT_LINKED_UNLOCK);
            break;
        case Q_EXIT_SIG:
            uart_linked_unlock_cancel();                    // 离开菜单：丢弃迟到的背板应答
            break;
        case Q_KEY_BOARD_PRESS_SIG:
            if (uart_linked_unlock_is_waiting())            // 等待结果期间不响应键盘，避免重复下发
                break;
            if (e->dynamic_[0] <= KEY_OK)
            {
                system_time_task_set_function_time(MENU_TIME_OUT_VAULE);

                if (KEY_OK == e->dynamic_[0])
                {
                    // 用户确认退出联动 → 下发 0x6D 给背板，等退出结果
                    uart_linked_unlock_start(LINKED_UNLOCK_ROLE_EXIT, NULL);
                    hmiTaskSetState(HMI_STATE_LINKED_UNLOCK_WAIT);
                    system_time_task_set_function_time(LINKED_UNLOCK_TIME_OUT);
                }
            }
            break;
        case Q_HANDLE_SIG:
            if (EVENT_RESULT_VOICE_TIME_OUT == e->dynamic_[0])
                // 等待背板结果期间保持完整等待时间，其它语音播完按普通停留超时
                system_time_task_set_function_time(uart_linked_unlock_is_waiting() ? LINKED_UNLOCK_TIME_OUT : MENU_TIME_OUT_VAULE);
            else if (EVENT_RESULT_FAIL == e->dynamic_[0])
            {
                state = Q_TRAN(lock_fsm_menu_system_settings); // 语音播完：回到系统设置菜单
            }
            else if (EVENT_RESULT_LINKED_UNLOCK_SUCCESS == e->dynamic_[0])
            {
                me->branch = (QStateHandler)(lock_fsm_menu_system_settings);
                state = Q_TRAN(lockFsmHandleSuccess);          // 背板上报退出成功：播"设置成功"
            }
            else if (EVENT_RESULT_LINKED_UNLOCK_FAIL == e->dynamic_[0])
            {
                me->branch = (QStateHandler)(lock_fsm_menu_system_settings);
                state = Q_TRAN(lockFsmLinkedUnlockFail);       // 指令异常/退出失败：播"设置失败"
            }
            break;
        case Q_USER_HANDLE_SIG:
            break;
        case Q_WORK_TIME_OUT_SIG:                               // 系统工时到期：菜单里不做动作（停留超时由功能超时管）
            break;
        case Q_FUNCTION_TIME_OUT_SIG:                           // 超时：播"操作超时"后回上级（更多设置）
            me->branch = (QStateHandler)(lock_fsm_menu_system_settings);
            state = Q_TRAN(lockFsmMenuTimeOut);
            break;
        default:
            break;
    }

    return state;
}
