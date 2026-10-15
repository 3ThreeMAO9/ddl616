/**
 * Copyright (c) 2026 GZ-OB, All rights reserved.
 * File name: fsm_state_device_test.c
 * Desc: 整机测试状态（按键输入 258# 进入）
 * Version: 1.0.0
 * Revision: James_Zhang
 * Date: 2026-10-14
 *
 * 说明：258# -> EVENT_RESULT_DEVICE_TEST_STEP1 -> 本状态（整机产测）。
 *       实现进度：**第一步（STEP1）按键检测已完成**，其余步骤待接。
 *
 *       第一步规则：必须严格按
 *         门铃 -> 1 -> 2 -> 3 -> 4 -> 5 -> 6 -> 7 -> 8 -> 9 -> * -> 0 -> #
 *       的顺序，把 13 个键各按一次（按键中断产生 Q_KEY_BOARD_PRESS_SIG）。
 *         - 按对了：继续等下一个键；
 *         - 顺序错：**无响应**（不播报、进度不变），继续等正确的键；
 *         - 13 个键全部按对：置位 DEVICE_TEST_FLAG_KEYPAD_ID + 播报"测试成功"。
 *       每一步的操作超时 = PRODUCE_DEVICE_TEST_TIME_OUT_MAX（30s），超时回待机。
 *       测试结果存放在参数区（produceInfo.deviceTest.flag），由 257# 读取。
 *
 *       后续步骤（指纹录入/删除、NFC、电机、复位键、门磁/过流等）在本状态内
 *       按 device_test_step_t（module_produce_test.h）逐个往下接。
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

/***************Function***************/

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

            key_step = 0;       // 第一步按键检测：进度清零，从门铃开始
            system_time_task_set_work_time(PRODUCE_DEVICE_TEST_TIME_OUT_MAX);
            OB_LOGW(TAG, "device test STEP1: press %u keys in order (bell,1..9,*,0,#)", DEVICE_TEST_KEY_SEQ_CNT);
            break;
        case Q_EXIT_SIG:
            key_step = 0;
            break;
        case Q_KEY_BOARD_PRESS_SIG:
            if (KEY_CNT > e->dynamic_[0])
            {
                if (device_test_key_seq[key_step] == e->dynamic_[0])
                {
                    if (KEY_NUM_13 == e->dynamic_[0])
                        uart_msg_bell(1, 1000);

                    hmiTaskSetState(HMI_STATE_DEVICE_TEST_KEY_PRESS);
                    key_step++;
                    OB_LOGI(TAG, "key ok: %u (%u/%u)", e->dynamic_[0], key_step, DEVICE_TEST_KEY_SEQ_CNT);

                    if (key_step >= DEVICE_TEST_KEY_SEQ_CNT)
                    {
                        // 13 个键全部按对：记录测试结果 + 播报"测试成功"
                        key_step = 0;
                        setDeviceTestFlagBit(DEVICE_TEST_FLAG_KEYPAD_ID);
                        hmiTaskSetState(HMI_STATE_DEVICE_TEST_STEP_SUCCESS);
                        OB_LOGW(TAG, "key board test PASS");
                        // TODO: STEP2
                    }
                }
                else
                {
                    // 顺序错：无响应（不播报、进度不变），继续等正确的键
                    OB_LOGI(TAG, "key out of order: %u (expect %u, step %u), ignored",
                            e->dynamic_[0], device_test_key_seq[key_step], key_step);
                }
            }
            break;
        case Q_HANDLE_SIG:
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
