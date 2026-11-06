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
#define TAG "fsm_create_linked_unlock"

/***************Variable***************/
QState lock_fsm_menu_create_linked_unlock(LockFsm *me, QEvent const *e)
{
    QState state = Q_IGNORED();     //  没有对应事件就返回Q_IGNORED()


    switch (e->sig){
        case Q_ENTRY_SIG:
            keyEventInit();
            keyTaskHandle(KEY_TYPE_KEY_BOARD, true);           //key board
            fp_task_set_mode(FP_MODE_IDLE);
            nfc_task_set_state(NFC_STATE_SLEEP);
            system_time_task_set_function_time(MENU_TIME_OUT_VAULE);
            hmiTaskSetState(HMI_STATE_CREATE_LINKED_UNLOCK);
            break;
        case Q_EXIT_SIG:
            uart_linked_unlock_cancel();                    // 离开菜单：丢弃迟到的背板应答
            break;
        case Q_KEY_BOARD_PRESS_SIG:
            if (uart_linked_unlock_is_waiting())            // 等待配对结果期间不响应键盘，避免重复下发
                break;
            if (e->dynamic_[0] <= KEY_OK)
            {
                hmiTaskSetState(HMI_STATE_KEY_BOARD_PRESS);
                system_time_task_set_function_time(MENU_TIME_OUT_VAULE);
                keyEventLinkedUnlock((e->dynamic_[0]), LINKED_UNLOCK_CREATE, 0);
            }
            break;
        case Q_HANDLE_SIG:
            if (EVENT_RESULT_VOICE_TIME_OUT == e->dynamic_[0])
                // 等待对方加入期间保持完整等待时间，其它语音播完按普通停留超时
                system_time_task_set_function_time(uart_linked_unlock_is_waiting() ? LINKED_UNLOCK_TIME_OUT : MENU_TIME_OUT_VAULE);
            else if (EVENT_RESULT_FAIL == e->dynamic_[0])
            {
                state = Q_TRAN(lock_fsm_menu_system_settings); // 输入密码空的时候,再按*键,回系统设置菜单
            }
            else if (EVENT_RESULT_LINKED_UNLOCK_SUCCESS == e->dynamic_[0])
            {
                me->branch = (QStateHandler)(lock_fsm_menu_system_settings);
                state = Q_TRAN(lockFsmHandleSuccess);          // 背板上报创建成功：播"设置成功"
            }
            else if (EVENT_RESULT_LINKED_UNLOCK_FAIL == e->dynamic_[0])
            {
                me->branch = (QStateHandler)(lock_fsm_menu_system_settings);
                state = Q_TRAN(lockFsmLinkedUnlockFail);       // 指令异常/配对失败：播"设置失败"
            }
            break;
        case Q_USER_HANDLE_SIG:
            break;
        case Q_LINK_KEY_SIG:
        {
            uint8_t pin[LINKED_UNLOCK_PIN_LEN];

            OB_LOGD_DUMP(&e->dynamic_[1],e->dynamic_[0]);

            if (e->dynamic_[0] < LINKED_UNLOCK_PIN_LEN)
                break;

            // 4 位配对码下发背板，等背板 ACK 后继续等 B 锁加入（结果由 0x6C 上报触发）
            memcpy(pin, &e->dynamic_[1], LINKED_UNLOCK_PIN_LEN);
            keyEventInit();                                 // 清掉键盘缓冲，避免再次按键叠加
            uart_linked_unlock_start(LINKED_UNLOCK_ROLE_CREATE, pin);
            hmiTaskSetState(HMI_STATE_LINKED_UNLOCK_WAIT);  // 播"配对中请稍候"
            system_time_task_set_function_time(LINKED_UNLOCK_TIME_OUT);
            break;
        }
        case Q_WORK_TIME_OUT_SIG:                               // 系统工时到期：菜单里不做动作（停留超时由功能超时管）
            break;
        case Q_FUNCTION_TIME_OUT_SIG:                               // 超时：播"操作超时"后回上级（更多设置）
            me->branch = (QStateHandler)(lock_fsm_menu_system_settings);
            state = Q_TRAN(lockFsmMenuTimeOut);
            break;
        default:
            break;
    }

    return state;
}
