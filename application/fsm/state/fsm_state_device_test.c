/**
 * Copyright (c) 2026 GZ-OB, All rights reserved.
 * File name: fsm_state_device_test.c
 * Desc: 整机测试状态（258# 进入）
 * Version: 1.4.0
 * Revision: James_Zhang
 * Date: 2026-10-15
 *
 * 流程：触摸键盘(13键,严格顺序) -> 指纹 -> [电机开锁] -> 刷卡 -> [电机关锁] -> 实体按键3个 -> 成功
 *      任一步失败 -> 失败播报；成功/失败都先恢复出厂设置再播报，语音播完自动休眠（不干等 30s 超时）。
 */

#include "fsm_state.h"
#include "state_inside.h"
#include "task_hmi.h"
#include "task_system_time.h"
#include "task_fingerprint.h"
#include "task_key.h"
#include "task_motor.h"
#include "task_nfc.h"

#include "module_produce_test.h"
#include "parameter.h"
#include "event.h"
#include "key_event.h"
#include "flash_data.h"
#include "lock_log.h"
#include "msg_protocol.h"       // uart_msg_bell（门铃命令 0x61，发给 WiFi 模块转后板）

#define OB_LOG_LEVEL OB_LOG_LEVEL_DEFAULT
#include "ob_log.h"
#define TAG "fsm_device_test"

/***************Variable***************/

// 触摸键盘按键顺序表（门铃 -> 1..9 -> * -> 0 -> #）
static const uint8_t device_test_key_seq[DEVICE_TEST_KEY_SEQ_CNT] = DEVICE_TEST_KEY_SEQ_TAB;

// 当前测试阶段（device_test_step_t 的子集，见 module_produce_test.h）
static uint8_t device_test_step = DEVICE_TEST_STEP_IDLE;

// 第四步三个实体按键的位掩码（顺序无所谓，每个键只记一次）
#define DEVICE_TEST_KEY_BIT_TAMPER      (0x01)        // 防撬按键
#define DEVICE_TEST_KEY_BIT_RESET       (0x02)        // 后板 RESET 键
#define DEVICE_TEST_KEY_BIT_SETTING     (0x04)        // 后板 SETTING 键
#define DEVICE_TEST_KEY_BIT_ALL         (0x01 | 0x02 | 0x04)

static uint8_t device_test_key_flag = 0;   // 第四步已按过的实体按键
static uint8_t key_step = 0;               // 第一步触摸键盘进度

/***************Function***************/

// 进入第一步：触摸键盘检测（严格按 门铃->1..9->*->0-># 顺序各按一次）
static void device_test_key_board_start(void)
{
    device_test_step = DEVICE_TEST_STEP_KEY_BOARD;
    key_step = 0;
    hmiTaskSetState(HMI_STATE_DEVICE_TEST_START);   // 键盘灯全亮 + 指示灯灭 + 配色复位
    system_time_task_set_work_time(PRODUCE_DEVICE_TEST_TIME_OUT_MAX);
}

// 进入第二步：指纹识别（只识别、不录入）
static void device_test_finger_start(void)
{
    device_test_step = DEVICE_TEST_STEP_VERIFY_FINGER;
    nfc_task_set_state(NFC_STATE_IDLE);
    fp_task_set_mode(FP_MODE_VERIFY);
    hmiTaskSetState(HMI_STATE_DEVICE_TEST_FINGER_ENROLL);
    system_time_task_set_work_time(PRODUCE_DEVICE_TEST_TIME_OUT_MAX);
}

// 进入第三步：NFC 检卡（只识别、不写卡）
static void device_test_nfc_start(void)
{
    device_test_step = DEVICE_TEST_STEP_VERIFY_NFC;
    fp_task_set_mode(FP_MODE_IDLE);
    nfc_task_set_state(NFC_STATE_DETECT);
    hmiTaskSetState(HMI_STATE_DEVICE_TEST_NFC);
    system_time_task_set_work_time(PRODUCE_DEVICE_TEST_TIME_OUT_MAX);
}

// 进入第四步：实体按键检测（防撬 + 后板 RESET + 后板 SETTING）
static void device_test_back_key_start(void)
{
    device_test_step = DEVICE_TEST_STEP_LOCK_KEY;  // 复用已有枚举
    nfc_task_set_state(NFC_STATE_IDLE);
    keyTaskHandle(KEY_TYPE_TAMPER_KEY, true);      // 要用防撬的长按事件
    device_test_key_flag = 0;
    hmiTaskSetState(HMI_STATE_DEVICE_TEST_KEY_TEST);
    system_time_task_set_work_time(PRODUCE_DEVICE_TEST_TIME_OUT_MAX);
}

// 离合电机：只驱动电机（250ms），不等待不提示，测试员在随后的人工操作中顺手拧把手判断
static void device_test_motor_handle(uint8_t handle)
{
    setDeviceTestFlagBit(DEVICE_TEST_FLAG_MOTOR_HANDLE_ID);
    motorTaskHandle(handle, 0);
}

// 测试结束统一恢复出厂（清用户/参数/开锁记录）。产测结果在 PRODUCE_DATA_PAGE，不受清除影响
static void device_test_factory_reset(void)
{
    flash_page_system_init();
    flash_data_init();
    lock_log_flash_clear();
}

// 某步失败：转"测试失败"播报，语音播完自动回休眠
static void device_test_fail(LockFsm *me, QState *state)
{
    device_test_step = DEVICE_TEST_STEP_IDLE;
    device_test_factory_reset();
    me->branch = (QStateHandler)(lock_fsm_sleep);
    *state = Q_TRAN(lockFsmDeviceTestFail);
}

// 第四步某个实体按键被按下；三个都按过 -> 本步通过并转"测试成功"播报
static void device_test_back_key_press(LockFsm *me, QState *state, uint8_t bit)
{
    if (device_test_key_flag & bit)
    {
        return;                                        // 只能按一次：重复按忽略
    }

    device_test_key_flag |= bit;
    hmiTaskSetState(HMI_STATE_DEVICE_TEST_KEY_TEST);   // 每按对一个键播一声"嘀"

    if (DEVICE_TEST_KEY_BIT_ALL == device_test_key_flag)
    {
        setDeviceTestFlagBit(DEVICE_TEST_FLAG_RESET_KEY_ID);  // 本步三个键统一用这个已有位
        device_test_step = DEVICE_TEST_STEP_IDLE;
        keyTaskHandle(KEY_TYPE_TAMPER_KEY, false);     // 关防撬，别把报警事件带出去
        writeDeviceTestResult(DEVICE_TEST_RESULT_PASS);
        device_test_factory_reset();
        me->branch = (QStateHandler)(lock_fsm_sleep);
        *state = Q_TRAN(lockFsmDeviceTestSuccess);
        // TODO: 门磁/过流/电流/BLE 接齐后在这里继续判结果
    }
}

// ------------------------------------------

QState lock_fsm_device_test(LockFsm *me, QEvent const *e)
{
    QState state = Q_IGNORED();

    switch (e->sig){
        case Q_ENTRY_SIG:
            keyEventInit();
            keyTaskHandle(KEY_TYPE_KEY_BOARD, true);
            keyTaskHandle(KEY_TYPE_TAMPER_KEY, false); // 前三步不检测防撬，避免误碰触发报警
            nfc_task_set_state(NFC_STATE_IDLE);
            fp_task_set_mode(FP_MODE_IDLE);
            hmiTaskSetAllowSelintFlag(false);

            device_test_key_flag = 0;
            // 新一轮测试：进度清零，结果先置"未通过"，全通过时才改 103
            clearDeviceTestFlagAllBit();
            writeDeviceTestResult(DEVICE_TEST_RESULT_FAIL);

            device_test_key_board_start();
            OB_LOGD(TAG, "device test start");
            break;
        case Q_EXIT_SIG:
            key_step = 0;
            device_test_step = DEVICE_TEST_STEP_IDLE;
            device_test_key_flag = 0;
            keyTaskHandle(KEY_TYPE_TAMPER_KEY, false);
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

                    if (key_step >= DEVICE_TEST_KEY_SEQ_CNT)
                    {
                        key_step = 0;
                        setDeviceTestFlagBit(DEVICE_TEST_FLAG_KEYPAD_ID);
                        device_test_finger_start();
                    }
                }
                // 顺序错：完全无响应，继续等正确的键
            }
            break;
        case Q_HANDLE_SIG:
            // 第四步三个实体按键，顺序无所谓、每个只记一次
            if (DEVICE_TEST_STEP_LOCK_KEY == device_test_step)
            {
                if (EVENT_RESULT_BREAK_LONG_PRESS == e->dynamic_[0])       // 防撬（长按 500ms）
                {
                    device_test_back_key_press(me, &state, DEVICE_TEST_KEY_BIT_TAMPER);
                }
                else if (EVENT_RESULT_BACK_KEY_SINGLE == e->dynamic_[0])   // 后板 RESET（单击）
                {
                    device_test_back_key_press(me, &state, DEVICE_TEST_KEY_BIT_RESET);
                }
                else if (EVENT_RESULT_ADD_ADMIN == e->dynamic_[0])         // 后板 SETTING（单击）
                {
                    device_test_back_key_press(me, &state, DEVICE_TEST_KEY_BIT_SETTING);
                }
            }
            break;
        case Q_USER_HANDLE_SIG:
            if (DEVICE_TEST_STEP_VERIFY_FINGER == device_test_step)
            {
                if (EVENT_RESULT_SUCCESS_VERIFY_USER == e->dynamic_[0])
                {
                    setDeviceTestFlagBit(DEVICE_TEST_FLAG_VERIFY_FINGERPRINT_ID);
                    device_test_motor_handle(MOTOR_HANDLE_OPEN);   // 开锁，此时拧把手应能动
                    device_test_nfc_start();
                }
                else if (EVENT_RESULT_FAIL_INVALID == e->dynamic_[0])
                {
                    device_test_fail(me, &state);
                }
            }
            else if (DEVICE_TEST_STEP_VERIFY_NFC == device_test_step)
            {
                if (EVENT_RESULT_SUCCESS_VERIFY_USER == e->dynamic_[0])
                {
                    setDeviceTestFlagBit(DEVICE_TEST_FLAG_NFC_ID);
                    device_test_motor_handle(MOTOR_HANDLE_CLOSE);  // 关锁，此时拧把手应拧不动
                    device_test_back_key_start();
                }
                else if (EVENT_RESULT_FAIL_INVALID == e->dynamic_[0])
                {
                    nfc_task_set_state(NFC_STATE_IDLE);
                    device_test_fail(me, &state);
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
