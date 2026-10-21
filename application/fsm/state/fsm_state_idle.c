#include "fsm_state.h"
#include "state_inside.h"

#include "event.h"
#include "key_event.h"
#include "parameter.h"
#include "user.h"
#include "msg_protocol.h"
#include "uart_protocol_blue_mac.h"
#include "lock_log.h"

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

// 双重验证：第一重已通过的钥匙类型 + 1（0 = 还没有第一重）。进入待机即复位，超时/离开也会清
static uint8_t dual_verify_type = 0;

/***************Function***************/


// ------------------------------------------

QState lock_fsm_idle(LockFsm *me, QEvent const *e)
{
    QState state = Q_IGNORED();     //  没有对应事件就返回Q_IGNORED()


    switch (e->sig){
        case Q_ENTRY_SIG:
            dual_verify_type = 0;       // 进入待机 = 新会话，双重验证重新开始
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

            // 后板版本 / 蓝牙 MAC：没取到就再问一次
            if (!uart_msg_back_version_valid()) {
                uart_msg_get_back_version();
            }
            if (!blue_mac_is_valid()) {
                uart_msg_blue_mac();
            }
            break;
        case Q_EXIT_SIG:
            dual_verify_type = 0;       // 离开待机（超时/开锁/失败）清掉第一重记录
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
            else if (EVENT_RESULT_REMOTE_ADD_FINGER == e->dynamic_[0])      // 云端(0x02)发起添加指纹
            {
                state = Q_TRAN(lock_fsm_remote_add_finger);
            }
            else if (EVENT_RESULT_REMOTE_ADD_CARD == e->dynamic_[0])        // 云端(0x02)发起添加卡片
            {
                state = Q_TRAN(lock_fsm_remote_add_card);
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
                lock_log_add_record(KIOT_TM_P_RECORD_EVENT_TYPE_BAO_JING_JI_LU,
                                    KIOT_TM_P_RECORD_ALARM_TYPE_QIAO_SUO_BAO_JING_SUO_BEI_QIAO_KAI,
                                    0, 0);
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
                    if (isEmptyKey(false))   // 初始化状态
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
                    if (isEmptyKey(false)){
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
                    hmiTaskSetState(HMI_STATE_JOIN_NET_START);
                    hmi_task_join_net_time(JOIN_NET_TIMEOUT);
                    uart_msg_join_net();
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
            else if (EVENT_RESULT_JOIN_NET_DONE == e->dynamic_[0])
            {
                // 配网结束：重置待机计时，保持唤醒等待 WiFi 同步数据
                system_time_task_set_work_time(WORK_TIME_OUT_VAULE);
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
                // 257#：出厂状态下查询产测结果并播报 100(未测试)/102(未通过)/103(通过)。
                // 非出厂状态此组合键不会被识别（见 key_event.c 的 isEmptyKey 判断），会作为密码处理
                hmiTaskSetState(HMI_STATE_DEVICE_TEST_RESULT);
            }
            else if (EVENT_RESULT_DEVICE_TEST_STEP1 == e->dynamic_[0])
            {
                state = Q_TRAN(lock_fsm_device_test);       // 258#：整机测试，第一步按键检测
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
                // 双重验证：两种不同类型才开锁（一次性密码不参与）
                if ((VERIFY_MODE_DUAL == readUserParameter(USER_PARA_VERIFY_MODE_ID))
                    && (VERIFY_SRC_ONE_TIME != e->dynamic_[3]))
                {
                    uint8_t key_type = e->dynamic_[3];      // verify_src_t

                    if (0 == dual_verify_type)
                    {
                        dual_verify_type = key_type + 1;    // 第一重：记下类型（+1，0 表示"未记录"）
                        hmiTaskSetState(HMI_STATE_DUAL_VERIFY_FIRST_OK);    // 蓝灯长亮，等第二重
                        OB_LOGI(TAG, "dual verify: first OK, type[%u]", key_type);
                        break;                              // 不开锁，留在待机等第二重
                    }

                    if (key_type == (uint8_t)(dual_verify_type - 1))
                    {
                        dual_verify_type = 0;               // 第二重与第一重同类型 -> 失败退出
                        me->branch = (QStateHandler)(lock_fsm_idle);
                        state = Q_TRAN(lockFsmDualVerifyFail);
                        break;
                    }

                    dual_verify_type = 0;                   // 第二重类型不同 -> 双重验证通过
                    OB_LOGI(TAG, "dual verify: second OK, type[%u]", key_type);
                }

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
