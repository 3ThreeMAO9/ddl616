/**
 * Copyright (c) 2026 GZ-OB, All rights reserved.
 * File name: fsm_state_device_test.c
 * Desc: 整机测试状态（按键输入 258# 进入）
 * Version: 1.2.0
 * Revision: James_Zhang
 * Date: 2026-10-15
 *
 * 说明：258# -> EVENT_RESULT_DEVICE_TEST_STEP1 -> 本状态（整机产测）。
 *       实现进度：**STEP1 按键检测、STEP2 指纹识别、STEP3 NFC 检卡已完成**，其余步骤待接。
 *       每步通过后置位对应的 deviceTest flag，并转入下一步；**只有最后一步跑完才播"测试成功"**。
 *
 *       第一步（STEP1）按键检测：必须严格按
 *         门铃 -> 1 -> 2 -> 3 -> 4 -> 5 -> 6 -> 7 -> 8 -> 9 -> * -> 0 -> #
 *       的顺序，把 13 个键各按一次（按键中断产生 Q_KEY_BOARD_PRESS_SIG）。
 *         - 按对的键：给按键反馈（键盘灯"灭 100ms 再亮起" + 指示灯换下一个颜色 + 一声"嘀"）；
 *           其中门铃键额外发门铃命令(0x61)给后板响铃；
 *         - 顺序错：**完全无响应**（不亮灯、不播报、进度不变），继续等正确的键
 *           （例如门铃已经按过，再按门铃就不响应，此时只有按 1 才有反馈）；
 *         - 13 个键全部按对：置位 DEVICE_TEST_FLAG_KEYPAD_ID，转入第二步。
 *
 *       第二步（STEP2）指纹识别：**只做识别、不录入**。播报"请按手指"，用户把手指放上模块：
 *         - 识别成功 -> 置位 DEVICE_TEST_FLAG_VERIFY_FINGERPRINT_ID，转入第三步；
 *         - 识别失败 -> 播报"测试失败"。
 *       走 FP_MODE_VERIFY（指纹验证回调），**不往指纹库里写任何东西**，也没有归属用户的概念。
 *
 *       第三步（STEP3）NFC 检卡：**只做识别、不写卡**。播报"请刷卡"，用户把卡贴近：
 *         - 刷卡识别成功 -> 置位 DEVICE_TEST_FLAG_NFC_ID，播报"测试成功"（当前流程最后一步）；
 *         - 识别失败     -> 播报"测试失败"。
 *       走 NFC_STATE_DETECT：驱动不做周期读卡，只在 ADC/LPCD 检到卡时才寻卡读 UID 并回调，
 *
 *       每一步的操作超时 = PRODUCE_DEVICE_TEST_TIME_OUT_MAX（30s），系统工时超时回待机。
 *       测试结果存放在参数区（produceInfo.deviceTest.flag），由 257# 读取。
 *
 *       后续步骤（指纹删除、电机/复位键/门磁/过流等）在本状态内按
 *       device_test_step_t（module_produce_test.h）逐个往下接。
 */

#include "fsm_state.h"
#include "state_inside.h"
#include "task_hmi.h"
#include "task_system_time.h"
#include "task_fingerprint.h"
#include "task_key.h"
#include "task_nfc.h"

#include "module_produce_test.h"
#include "parameter.h"
#include "event.h"
#include "key_event.h"
#include "msg_protocol.h"       // uart_msg_bell（门铃命令 0x61，发给 WiFi 模块转后板）

#define OB_LOG_LEVEL OB_LOG_LEVEL_DEFAULT
#include "ob_log.h"
#define TAG "fsm_device_test"

/***************Variable***************/

// 整机测试第一步的按键顺序表（门铃 -> 1..9 -> * -> 0 -> #）
static const uint8_t device_test_key_seq[DEVICE_TEST_KEY_SEQ_CNT] = DEVICE_TEST_KEY_SEQ_TAB;

// 当前测试阶段（用 device_test_step_t 的子集，见 module_produce_test.h）
static uint8_t device_test_step = DEVICE_TEST_STEP_IDLE;

/***************Function***************/

// 进入第二步：指纹识别（播"请按手指"，等用户放手指）
// 只做识别：切 FP_MODE_VERIFY，不写指纹库、不涉及归属用户
static void device_test_finger_start(void)
{
    device_test_step = DEVICE_TEST_STEP_VERIFY_FINGER;
    nfc_task_set_state(NFC_STATE_IDLE);
    fp_task_set_mode(FP_MODE_VERIFY);
    hmiTaskSetState(HMI_STATE_DEVICE_TEST_FINGER_ENROLL);       // 播"请按手指"
    system_time_task_set_work_time(PRODUCE_DEVICE_TEST_TIME_OUT_MAX);
    system_time_task_set_function_time(PRODUCE_DEVICE_TEST_TIME_OUT_MAX);
    OB_LOGW(TAG, "device test STEP2: finger verify");
}

// 进入第三步：NFC 检卡（播"请刷卡"，等用户刷卡）
// 用 NFC_STATE_DETECT：驱动侧不做周期读卡，只有 ADC/LPCD 检到卡才读一次卡
static void device_test_nfc_start(void)
{
    device_test_step = DEVICE_TEST_STEP_VERIFY_NFC;
    fp_task_set_mode(FP_MODE_IDLE);
    nfc_task_set_state(NFC_STATE_DETECT);       // 只靠检卡触发读卡，不主动定时读卡
    hmiTaskSetState(HMI_STATE_DEVICE_TEST_NFC);                 // 播"请刷卡"
    system_time_task_set_work_time(PRODUCE_DEVICE_TEST_TIME_OUT_MAX);
    system_time_task_set_function_time(PRODUCE_DEVICE_TEST_TIME_OUT_MAX);
    OB_LOGW(TAG, "device test STEP3: nfc verify");
}

// ------------------------------------------

QState lock_fsm_device_test(LockFsm *me, QEvent const *e)
{
    static uint8_t key_step = 0;        // 当前等着按第几个键（0 ~ DEVICE_TEST_KEY_SEQ_CNT-1）
    QState state = Q_IGNORED();     //  没有对应事件就返回Q_IGNORED()

    switch (e->sig){
        case Q_ENTRY_SIG:
            keyEventInit();
            keyTaskHandle(KEY_TYPE_KEY_BOARD, true);           //key board
            nfc_task_set_state(NFC_STATE_IDLE);
            fp_task_set_mode(FP_MODE_IDLE);

            hmiTaskSetAllowSelintFlag(false);
            hmiTaskSetState(HMI_STATE_DEVICE_TEST_START);   // 键盘灯全亮 + 按键配色复位

            device_test_step = DEVICE_TEST_STEP_KEY_BOARD;  // 从第一步开始
            key_step = 0;       // 第一步按键检测：进度清零，从门铃开始
            system_time_task_set_work_time(PRODUCE_DEVICE_TEST_TIME_OUT_MAX);
            OB_LOGW(TAG, "device test STEP1: press %u keys in order (bell,1..9,*,0,#)", DEVICE_TEST_KEY_SEQ_CNT);
            break;
        case Q_EXIT_SIG:
            key_step = 0;
            device_test_step = DEVICE_TEST_STEP_IDLE;
            break;
        case Q_KEY_BOARD_PRESS_SIG:
            if ((DEVICE_TEST_STEP_KEY_BOARD == device_test_step) && (KEY_CNT > e->dynamic_[0]))
            {
                if (device_test_key_seq[key_step] == e->dynamic_[0])
                {
                    if (KEY_NUM_13 == e->dynamic_[0])
                        uart_msg_bell(1, 0);     // 门铃：发命令给后板响铃

                    hmiTaskSetState(HMI_STATE_DEVICE_TEST_KEY_PRESS);
                    key_step++;
                    OB_LOGI(TAG, "key ok: %u (%u/%u)", e->dynamic_[0], key_step, DEVICE_TEST_KEY_SEQ_CNT);

                    if (key_step >= DEVICE_TEST_KEY_SEQ_CNT)
                    {
                        // 13 个键全部按对：记录结果，转入第二步（"测试成功"留到最后播）
                        key_step = 0;
                        setDeviceTestFlagBit(DEVICE_TEST_FLAG_KEYPAD_ID);
                        OB_LOGW(TAG, "key board test PASS");
                        device_test_finger_start();
                    }
                }
                else
                {
                    // 顺序错：完全无响应（不亮灯、不播报、进度不变），继续等正确的键
                    OB_LOGI(TAG, "key out of order: %u (expect %u, step %u), ignored",
                            e->dynamic_[0], device_test_key_seq[key_step], key_step);
                }
            }
            break;
        case Q_HANDLE_SIG:
            break;
        case Q_USER_HANDLE_SIG:
            if (DEVICE_TEST_STEP_VERIFY_FINGER == device_test_step)
            {
                // 第二步：指纹识别（事件来自 task_fingerprint 的验证回调）
                if (EVENT_RESULT_SUCCESS_VERIFY_USER == e->dynamic_[0])
                {
                    // 识别成功 = 第二步通过 -> 转第三步
                    setDeviceTestFlagBit(DEVICE_TEST_FLAG_VERIFY_FINGERPRINT_ID);
                    OB_LOGW(TAG, "finger test PASS -> nfc test");
                    device_test_nfc_start();
                }
                else if (EVENT_RESULT_FAIL_INVALID == e->dynamic_[0])
                {
                    hmiTaskSetState(HMI_STATE_DEVICE_TEST_STEP_FAIL);       // 播"测试失败"
                    OB_LOGW(TAG, "finger test FAIL");
                    device_test_step = DEVICE_TEST_STEP_IDLE;
                }
            }
            else if (DEVICE_TEST_STEP_VERIFY_NFC == device_test_step)
            {
                // 第三步：NFC 检卡（事件来自 task_nfc 的检卡回调）
                if (EVENT_RESULT_SUCCESS_VERIFY_USER == e->dynamic_[0])
                {
                    // 刷卡识别成功 = 第三步通过 = 当前流程跑完，播"测试成功"
                    setDeviceTestFlagBit(DEVICE_TEST_FLAG_NFC_ID);
                    device_test_step = DEVICE_TEST_STEP_IDLE;
                    hmiTaskSetState(HMI_STATE_DEVICE_TEST_STEP_SUCCESS);    // 播"测试成功"
                    OB_LOGW(TAG, "nfc test PASS -> device test done");
                    // TODO: 下一步（电机/复位键…）
                }
                else if (EVENT_RESULT_FAIL_INVALID == e->dynamic_[0])
                {
                    hmiTaskSetState(HMI_STATE_DEVICE_TEST_STEP_FAIL);       // 播"测试失败"
                    OB_LOGW(TAG, "nfc test FAIL");
                    device_test_step = DEVICE_TEST_STEP_IDLE;
                }
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
