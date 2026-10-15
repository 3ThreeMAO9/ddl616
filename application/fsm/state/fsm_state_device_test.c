/**
 * Copyright (c) 2026 GZ-OB, All rights reserved.
 * File name: fsm_state_device_test.c
 * Desc: 整机测试状态（按键输入 258# 进入）
 * Version: 1.3.0
 * Revision: James_Zhang
 * Date: 2026-10-15
 *
 * 说明：258# -> EVENT_RESULT_DEVICE_TEST_STEP1 -> 本状态（整机产测）。
 *       实现进度：**STEP1 触摸键盘、STEP2 指纹识别、STEP3 NFC 检卡、STEP4 实体按键（防撬+后板）
 *       + 离合电机测试（开/关）已完成**，其余步骤待接。
 *       每步通过后置位对应的 deviceTest flag，并转入下一步；**只有最后一步跑完才播"测试成功"**。
 *
 *       离合电机测试：**门锁无法感知"拧把手"动作**（没有把手位置传感器），所以融进相邻步骤 ——
 *         - 指纹验证通过后 -> 驱动电机**开锁**，离合松开；测试人员在随后刷卡的过程中顺手拧把手，
 *           此时**应能带动锁芯**；
 *         - 刷卡验证通过后 -> 驱动电机**关锁**，离合咬合；测试人员在随后按实体键的过程中顺手拧把手，
 *           此时**应拧不动**。
 *       只驱动电机（各 250ms），**不额外等待、不做灯效/语音提示**，电机动作时间完全被人工操作
 *       时间掩盖，不增加工时；两个检查点都只置 DEVICE_TEST_FLAG_MOTOR_HANDLE_ID，
 *       **是否合格由测试人员现场判断**。
 *
 *       工时相关：任一步失败 -> 播"测试失败"，停留 DEVICE_TEST_FAIL_EXIT_TIME(2s) 后自动退出本状态，
 *       **不干等 30s 工时超时**（产测按工时计费，失败台的 30s 空等是最大的浪费）。
 *
 *       第一步（STEP1）触摸键盘检测：必须严格按
 *         门铃 -> 1 -> 2 -> 3 -> 4 -> 5 -> 6 -> 7 -> 8 -> 9 -> * -> 0 -> #
 *       的顺序，把 13 个触摸键各按一次（触摸芯片上报产生 Q_KEY_BOARD_PRESS_SIG）。
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
 *       第四步（STEP4）实体按键检测：**只检测按键通路，不触发任何业务动作**。检测三个实体按键：
 *         - 防撬按键（EVENT_RESULT_BREAK_LONG_PRESS，按住约 500ms 才算一次，只触发一次）；
 *         - 后板 RESET 键（EVENT_RESULT_BACK_KEY_SINGLE，后板单击上报，仅本状态处理）；
 *         - 后板 SETTING 键（EVENT_RESULT_ADD_ADMIN，后板单击上报）。
 *         三个键**顺序无所谓**，每个键**只记一次**（重复按忽略），三个都按过 -> 置位
 *         DEVICE_TEST_FLAG_RESET_KEY_ID，播"测试成功"（当前流程最后一步）。
 *         30s 内没按齐 -> 工时超时回待机（不会主动判失败）。
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
#include "task_motor.h"
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

// 第四步三个实体按键的位掩码（顺序无所谓，每个键只记一次）
#define DEVICE_TEST_KEY_BIT_TAMPER      (0x01)        // 防撬按键
#define DEVICE_TEST_KEY_BIT_RESET       (0x02)        // 后板 RESET 键
#define DEVICE_TEST_KEY_BIT_SETTING     (0x04)        // 后板 SETTING 键
#define DEVICE_TEST_KEY_BIT_ALL         (0x01 | 0x02 | 0x04)

// 某步失败后：播完"测试失败"停留这么久就自动退出产测（不干等 30s 工时超时，省工时）
#define DEVICE_TEST_FAIL_EXIT_TIME      (2000)        //unit: ms

// 第四步已按过的实体按键
static uint8_t device_test_key_flag = 0;

// 第一步触摸键盘进度（当前等着按第几个键，0 ~ DEVICE_TEST_KEY_SEQ_CNT-1）
static uint8_t key_step = 0;

/***************Function***************/

// 进入第一步：触摸键盘检测（必须严格按 门铃->1..9->*->0-># 顺序各按一次）
static void device_test_key_board_start(void)
{
    device_test_step = DEVICE_TEST_STEP_KEY_BOARD;
    key_step = 0;                                           // 进度清零，从门铃开始
    hmiTaskSetState(HMI_STATE_DEVICE_TEST_START);           // 键盘灯全亮 + 指示灯灭 + 按键配色复位
    system_time_task_set_work_time(PRODUCE_DEVICE_TEST_TIME_OUT_MAX);
    system_time_task_set_function_time(PRODUCE_DEVICE_TEST_TIME_OUT_MAX);
    OB_LOGD(TAG, "STEP1: touch keys in order (bell,1..9,*,0,#)");
}

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
    OB_LOGD(TAG, "STEP2: finger verify");
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
    OB_LOGD(TAG, "STEP3: nfc verify");
}

// 进入第四步：实体按键测试（防撬键 + 后板 RESET 键 + 后板 SETTING 键，顺序无所谓，每个键只记一次）
static void device_test_back_key_start(void)
{
    device_test_step = DEVICE_TEST_STEP_LOCK_KEY;      // 复用已有枚举：本步测的就是锁体上的实体按键
    nfc_task_set_state(NFC_STATE_IDLE);
    keyTaskHandle(KEY_TYPE_TAMPER_KEY, true);          // 确保防撬键扫描是开的（要用它的长按事件）
    device_test_key_flag = 0;
    hmiTaskSetState(HMI_STATE_DEVICE_TEST_KEY_TEST);   // 一声"嘀"，提示开始按实体按键
    system_time_task_set_work_time(PRODUCE_DEVICE_TEST_TIME_OUT_MAX);
    system_time_task_set_function_time(PRODUCE_DEVICE_TEST_TIME_OUT_MAX);
    OB_LOGD(TAG, "STEP4: tamper + back key");
}

// 离合电机测试(开锁)：指纹验证通过后驱动电机开锁，离合松开，测试人员在随后刷卡过程中
// 顺手拧把手，此时应能带动锁芯。门锁无法感知拧把手，所以只驱动电机，不等待、不做提示
static void device_test_motor_open(void)
{
    setDeviceTestFlagBit(DEVICE_TEST_FLAG_MOTOR_HANDLE_ID);    // 已执行离合电机(开锁)动作
    motorTaskHandle(MOTOR_HANDLE_OPEN, 0);                    // 电机开锁，动作时间 250ms
    OB_LOGD(TAG, "MOTOR: open");
}

// 离合电机测试(关锁)：刷卡验证通过后驱动电机关锁，离合咬合，测试人员在随后按实体键过程中
// 顺手拧把手，此时应拧不动。同样只驱动电机，不等待、不做提示
static void device_test_motor_lock(void)
{
    setDeviceTestFlagBit(DEVICE_TEST_FLAG_MOTOR_HANDLE_ID);    // 已执行离合电机(关锁)动作
    motorTaskHandle(MOTOR_HANDLE_CLOSE, 0);                   // 电机关锁，动作时间 250ms
    OB_LOGD(TAG, "MOTOR: close");
}

// 第四步某个实体按键被按下：顺序无所谓、每个键只记一次；三个都按过 -> 本步通过，播"测试成功"
static void device_test_back_key_press(uint8_t bit)
{
    if (device_test_key_flag & bit)
    {
        return;                                        // 只能按一次：重复按忽略
    }

    device_test_key_flag |= bit;
    hmiTaskSetState(HMI_STATE_DEVICE_TEST_KEY_TEST);   // 每按对一个键播一声"嘀"

    if (DEVICE_TEST_KEY_BIT_ALL == device_test_key_flag)    // 三个键都按过 -> 本步通过
    {
        setDeviceTestFlagBit(DEVICE_TEST_FLAG_RESET_KEY_ID);  // 本步三个键统一用这个已有位上报
        device_test_step = DEVICE_TEST_STEP_IDLE;
        keyTaskHandle(KEY_TYPE_TAMPER_KEY, false);            // 本步结束关防撬，别把长按报警事件带出去
        hmiTaskSetState(HMI_STATE_DEVICE_TEST_STEP_SUCCESS);  // 播"测试成功"（当前流程最后一步）
        // TODO: 下一步（电机/门磁/过流…）
    }
}

// ------------------------------------------

QState lock_fsm_device_test(LockFsm *me, QEvent const *e)
{
    QState state = Q_IGNORED();     //  没有对应事件就返回Q_IGNORED()

    switch (e->sig){
        case Q_ENTRY_SIG:
            keyEventInit();
            keyTaskHandle(KEY_TYPE_KEY_BOARD, true);           //key board
            keyTaskHandle(KEY_TYPE_TAMPER_KEY, false);         // 前三步不检测防撬（第四步才打开），避免误碰触发报警
            nfc_task_set_state(NFC_STATE_IDLE);
            fp_task_set_mode(FP_MODE_IDLE);
            hmiTaskSetAllowSelintFlag(false);

            device_test_key_flag = 0;
            device_test_key_board_start();                      // 直接进入第一步触摸键盘
            break;
        case Q_EXIT_SIG:
            key_step = 0;
            device_test_step = DEVICE_TEST_STEP_IDLE;
            device_test_key_flag = 0;
            keyTaskHandle(KEY_TYPE_TAMPER_KEY, false);         // 退出产测关闭防撬，别把长按报警事件带出去
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
                        // 13 个键全部按对：记录结果，转入第二步（"测试成功"留到最后播）
                        key_step = 0;
                        setDeviceTestFlagBit(DEVICE_TEST_FLAG_KEYPAD_ID);
                        device_test_finger_start();
                    }
                }
                // 顺序错：完全无响应（不亮灯、不播报、进度不变），继续等正确的键
            }
            break;
        case Q_HANDLE_SIG:
            // 第四步：三个按键（防撬 / 后板 RESET / 后板 SETTING），顺序无所谓、每个只记一次
            if (DEVICE_TEST_STEP_LOCK_KEY == device_test_step)
            {
                if (EVENT_RESULT_BREAK_LONG_PRESS == e->dynamic_[0])       // 防撬键（按住约 500ms，只触发一次）
                {
                    // 这里复用防撬的报警事件做按键检测：产测期间它只被本状态消费，不会触发防撬报警
                    device_test_back_key_press(DEVICE_TEST_KEY_BIT_TAMPER);
                }
                else if (EVENT_RESULT_BACK_KEY_SINGLE == e->dynamic_[0])   // 后板 RESET 键（单击上报，仅本状态处理）
                {
                    device_test_back_key_press(DEVICE_TEST_KEY_BIT_RESET);
                }
                else if (EVENT_RESULT_ADD_ADMIN == e->dynamic_[0])         // 后板 SETTING 键（单击上报）
                {
                    device_test_back_key_press(DEVICE_TEST_KEY_BIT_SETTING);
                }
            }
            break;
        case Q_USER_HANDLE_SIG:
            if (DEVICE_TEST_STEP_VERIFY_FINGER == device_test_step)
            {
                // 第二步：指纹识别（事件来自 task_fingerprint 的验证回调）
                if (EVENT_RESULT_SUCCESS_VERIFY_USER == e->dynamic_[0])
                {
                    // 识别成功 = 第二步通过 -> 驱动电机开锁做离合测试(应能带动锁芯) -> 转第三步
                    setDeviceTestFlagBit(DEVICE_TEST_FLAG_VERIFY_FINGERPRINT_ID);
                    device_test_motor_open();
                    device_test_nfc_start();
                }
                else if (EVENT_RESULT_FAIL_INVALID == e->dynamic_[0])
                {
                    hmiTaskSetState(HMI_STATE_DEVICE_TEST_STEP_FAIL);       // 播"测试失败"
                    device_test_step = DEVICE_TEST_STEP_IDLE;
                    system_time_task_set_work_time(DEVICE_TEST_FAIL_EXIT_TIME);  // 播完 2s 自动退出，不等 30s
                }
            }
            else if (DEVICE_TEST_STEP_VERIFY_NFC == device_test_step)
            {
                // 第三步：NFC 检卡（事件来自 task_nfc 的检卡回调）
                if (EVENT_RESULT_SUCCESS_VERIFY_USER == e->dynamic_[0])
                {
                    // 刷卡识别成功 = 第三步通过 -> 关锁做离合测试(应拧不动) -> 转第四步
                    setDeviceTestFlagBit(DEVICE_TEST_FLAG_NFC_ID);
                    device_test_motor_lock();
                    device_test_back_key_start();
                }
                else if (EVENT_RESULT_FAIL_INVALID == e->dynamic_[0])
                {
                    hmiTaskSetState(HMI_STATE_DEVICE_TEST_STEP_FAIL);       // 播"测试失败"
                    device_test_step = DEVICE_TEST_STEP_IDLE;
                    nfc_task_set_state(NFC_STATE_IDLE);
                    system_time_task_set_work_time(DEVICE_TEST_FAIL_EXIT_TIME);  // 播完 2s 自动退出，不等 30s
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
