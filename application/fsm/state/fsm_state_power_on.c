#include "fsm_state.h"
#include "state_inside.h"
#include "task_hmi.h"
#include "task_system_time.h"
#include "task_key.h"
#include "task_nfc.h"
#include "task_fingerprint.h"
#include "task_battery.h"
#include "task_motor.h"

#include "event.h"
#include "key_event.h"
#include "parameter.h"
#include "msg_protocol.h"
#include "encrypt.h"

#define OB_LOG_LEVEL OB_LOG_LEVEL_DEFAULT
#include "ob_log.h"
#define TAG "fsm_power_on"

/***************Variable***************/


/***************Function***************/


// ------------------------------------------

// 非 static：它是 Q_TRAN 的目标状态，引擎的名字表里要引用（见 qp_port.c）
QState lock_fsm_power_on(LockFsm *me, QEvent const *e)
{
    QState state = Q_IGNORED();     //  没有对应事件就返回Q_IGNORED()


    switch (e->sig){
        case Q_ENTRY_SIG:
            // 上电：生成门锁密钥并广播给 WiFi（0x70，出厂密钥加密，等 0xF0 回执）
            lock_key_generate();
            uart_msg_send_lock_key();

            me->verify_fail_cnt = readUserParameter(USER_PARA_VERIFY_FAIL_CNT_ID);
            if (me->verify_fail_cnt)
                me->verify_fail_time = hal_get_rtc_work_time();
            keyEventInit();
            keyTaskHandle(KEY_TYPE_KEY_BOARD, false);           //key board
            fp_task_set_mode(FP_MODE_SLEEP);
            nfc_task_set_state(NFC_STATE_SLEEP);

            system_time_task_set_function_time(1200);
            system_time_task_set_work_time(1000);
            hmiTaskSetAllowSelintFlag(true);
            // 低电系统锁定：不驱动电机
            if (BATTERY_STATE_LOW_SYSTEM_LOCK == batteryTaskReadState())
            {
                break;
            }

            hmiTaskSetState(HMI_STATE_POWER_ON);

            // 通道模式保持开锁，否则默认关锁
            if (Enabled == readUserParameter(USER_PARA_CHANNEL_MODE_ID))
            {
                motorTaskHandle(MOTOR_HANDLE_OPEN, 0);
            }
            else
            {
                motorTaskHandle(MOTOR_HANDLE_CLOSE, 0);
            }
            break;
        case Q_EXIT_SIG:
            break;
        case Q_HANDLE_SIG:
            break;
        case Q_FUNCTION_TIME_OUT_SIG:
            break;
        case Q_WORK_TIME_OUT_SIG:
            if (BATTERY_STATE_LOW_SYSTEM_LOCK == batteryTaskReadState())
            {
                state = Q_TRAN(lock_fsm_low_power_system_lock);
                break;
            }
            else if (Enabled == readUserParameter(USER_PARA_SYSTEM_LOCK_ID))
            {
                state = Q_TRAN(lock_fsm_system_lock);
            }
            else if (Enabled == readUserParameter(USER_PARA_BREAK_ID))
            {
                keyTaskHandle(KEY_TYPE_TAMPER_KEY, false);
                hmi_task_tamper_warn_time(TAMPER_WARN_KEEP_TIME);
            }
            state = Q_TRAN(lock_fsm_idle);
            break;
        default:
            break;
    }

    return state;
}

QState lock_fsm_init(LockFsm *me, QEvent const *e)
{
    (void)e;
    

    return Q_TRAN(lock_fsm_power_on);
}
