#include "fsm_state.h"
#include "state_inside.h"

#include "event.h"
#include "key_event.h"
#include "parameter.h"
#include "user.h"
#include "msg_protocol.h"

#include "task_sleep.h"
#include "task_system_time.h"
#include "task_hmi.h"

#include "task_fingerprint.h"
#include "task_motor.h"
#include "task_key.h"
#include "task_nfc.h"
#include "task_battery.h"

#define OB_LOG_LEVEL OB_LOG_LEVEL_DEFAULT
#include "ob_log.h"
#define TAG "fsm_idle"

/***************Variable***************/


/***************Function***************/


// ------------------------------------------

QState lock_fsm_idle(LockFsm *me, QEvent const *e)
{
    QState state = Q_IGNORED();     //  没有对应事件就返回Q_IGNORED()

    OB_LOGD(TAG, "Fsm_state[%s], Event[%u, %u]", "idle", e->sig, e->dynamic_[0]);

    switch (e->sig){
        case Q_ENTRY_SIG:
            keyTaskInit(KEY_TYPE_KEY_BOARD);
            keyTaskHandle(KEY_TYPE_KEY_BOARD, true);           //key board
            fp_task_set_mode(FP_MODE_VERIFY);
            nfc_task_set_state(NFC_STATE_VERIFY);
            hmiTaskSetAllowSelintFlag(true);
            // face_task_set_mode(FACE_TASK_MODE_DEFAULT);
            if (BATTERY_STATE_LOW_SYSTEM_LOCK == batteryTaskReadState()){
                state = Q_TRAN(lock_fsm_low_power_system_lock);
            }
            else{
                system_time_task_set_work_time(WORK_TIME_OUT_VAULE);
                if (me->idle_sleep_flag == true)
                    me->idle_sleep_flag = false;
                else
                    hmiTaskSetState(HMI_STATE_KEY_BOARD_LED_ON);
            }
            break;
        case Q_EXIT_SIG:
            break;
        case Q_KEY_BOARD_PRESS_SIG:
            system_time_task_set_work_time(WORK_TIME_OUT_VAULE);
            if (KEY_NUM_13 == e->dynamic_[0])   // 门铃
            {
                hmiTaskSetState(HMI_STATE_BELL);
                uart_msg_bell(2, 1000);
            }
            else
            {
                if ((TRUN_OFF == hmi_task_read_keyboard_state()) && (e->dynamic_[0] < KEY_CNT)){
                    hmiTaskSetState(HMI_STATE_KEY_BOARD_WAKE_UP);
                }
                else {
                    hmiTaskSetState(HMI_STATE_KEY_BOARD_PRESS);
                    keyEventVerifyUser(e->dynamic_[0]);
                }
            }
            break;
        case Q_HANDLE_SIG:
            if (EVENT_RESULT_VOICE_TIME_OUT == e->dynamic_[0])
            {
                system_time_task_set_work_time(WORK_TIME_OUT_VAULE);
            }
            else if (EVENT_RESULT_VERIFY_INPUT_ERROR == e->dynamic_[0])
            {
                me->branch = (QStateHandler)(lock_fsm_idle);
                state = Q_TRAN(lockFsmInputError);
            }
            else if ((EVENT_RESULT_FAIL_TOO_SHORT == e->dynamic_[0]) || (EVENT_RESULT_FAIL_TOO_LONG == e->dynamic_[0]))
            {
                me->branch = (QStateHandler)(lock_fsm_idle);
                state = Q_TRAN(lockFsmVerifyFail);
            }
            else if (EVENT_RESULT_VOICE_MODE == e->dynamic_[0])
            {
                system_time_task_set_work_time(WORK_TIME_OUT_VAULE);
                keyEventInit(); // 清空已输入的密码
                if (readUserParameter(USER_PARA_SILENT_MODE_ID)){
                    setUserParameter(USER_PARA_SILENT_MODE_ID, Disabled);
                }
                else{
                    setUserParameter(USER_PARA_SILENT_MODE_ID, Enabled);
                }
                me->branch = (QStateHandler)(lock_fsm_idle);
                state = Q_TRAN(lockFsmHandleVoiceModeSuccess);
            }
            else if (EVENT_RESULT_BREAK_LONG_PRESS == e->dynamic_[0])
            {
                keyTaskHandle(KEY_TYPE_TAMPER_KEY, false);
                hmi_task_tamper_warn_time(TAMPER_WARN_KEEP_TIME);
            }
            else if (EVENT_RESULT_BREAK_WARN == e->dynamic_[0])
            {
                hmiTaskSetState(HMI_STATE_TAMPER_WARN);
                system_time_task_set_work_time(WORK_TIME_OUT_VAULE);
            }
            else if (EVENT_RESULT_RESET == e->dynamic_[0])
            {
                state = Q_TRAN(lock_fsm_reset);
            }
            else if (EVENT_RESULT_FAIL == e->dynamic_[0])
            {
                // state = Q_TRAN(lock_fsm_sleep); // 输入密码空的时候,再按*键
            }
            else if (EVENT_RESULT_ADD_ADMIN == e->dynamic_[0])
            {
                if (is_device_locked()) {
                    me->branch = (QStateHandler)(lock_fsm_sleep);
                    state = Q_TRAN(lockFsmHandleFail);
                }
                else {
                    if (isEmptyUser(false))   // 初始化状态
                        state = Q_TRAN(lock_fsm_menu_modfiy_admin_pin);
                }
            }
            else if (EVENT_RESULT_ENTER_LOCAL_MENU == e->dynamic_[0])
            {
                if (is_device_locked()) {
                    me->branch = (QStateHandler)(lock_fsm_sleep);
                    state = Q_TRAN(lockFsmHandleFail);
                }
                else {
                    // 初始化状态下，单击和双击SET都进入修改管理员密码
                    if (isEmptyUser(false)){
                        state = Q_TRAN(lock_fsm_menu_modfiy_admin_pin);
                    }
                    else{
                        hmiTaskSetState(HMI_STATE_ENTER_ADMIN_MODE);
                        state = Q_TRAN(lock_fsm_verify_admin);
                    }
                }
            }
            else if (EVENT_RESULT_ENTER_NET_CONFIG == e->dynamic_[0])
            {
                if (is_device_locked()) {
                    me->branch = (QStateHandler)(lock_fsm_sleep);
                    state = Q_TRAN(lockFsmHandleFail);
                }
                else {
                    hmi_task_join_net_time(JOIN_NET_TIMEOUT);
                    state = Q_TRAN(lock_fsm_sleep);
                }
            }
            else if (EVENT_RESULT_ACTIVECODE_SUCCESS == e->dynamic_[0])
            {
                hmiTaskSetState(HMI_STATE_ACTIVECODE_SUCCESS);
            }
            else if (EVENT_RESULT_JOIN_NET_MODE == e->dynamic_[0])
            {
                hmiTaskSetState(HMI_STATE_JOIN_NET);
            }
            else if (EVENT_RESULT_VERSION == e->dynamic_[0])
            {
                hmiTaskSetState(HMI_STATE_VERSION);
            }
            else if (EVENT_RESULT_BLUE_MAC == e->dynamic_[0])
            {
                hmiTaskSetState(HMI_STATE_BLUE_MAC);
            }
            else if (EVENT_RESULT_SN == e->dynamic_[0])
            {

            }
            else if (EVENT_RESULT_PID == e->dynamic_[0])
            {
                
            }
            else if (EVENT_RESULT_DEVICE_TEST_RESULT == e->dynamic_[0])
            {

            }
            else if (EVENT_RESULT_AGING_TEST == e->dynamic_[0])
            {
                state = Q_TRAN(lock_fsm_aging_test);
            }
#ifdef DEFINE_299_CHECK_TIME
            else if (EVENT_RESULT_TIME == e->dynamic_[0])
            {
                hmiTaskSetState(HMI_STATE_TIME);
            }
#endif
            // if (e->dynamic_[0] == HANDLE_EVENT_UART_RX){
            //     system_time_task_set_work_time(WORK_TIME_OUT_VAULE);
            // }
            // else if (e->dynamic_[0] == HANDLE_EVENT_SLEEP){
            //     state = Q_TRAN(lock_fsm_sleep);
            // }
            break;
        case Q_USER_HANDLE_SIG:
            me->branch = (QStateHandler)(lock_fsm_idle);

            if (EVENT_RESULT_SUCCESS_VERIFY_USER == (e->dynamic_[0]))
            {
                OB_LOGI(TAG, "user_id [%u]", ((uint16_t)e->dynamic_[1] << 8) | e->dynamic_[2]);
                hmi_task_tamper_warn_time(0);
                motorTaskHandle(MOTOR_HANDLE_UNLOCK, 0);
                me->branch = (QStateHandler)(lock_fsm_sleep);
                state = Q_TRAN(lockFsmVerifyUserSuccess);
            }
            else if (EVENT_RESULT_FAIL_INVALID == (e->dynamic_[0]))
            {
                me->branch = (QStateHandler)(lock_fsm_idle);
                state = Q_TRAN(lockFsmVerifyFail);
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
