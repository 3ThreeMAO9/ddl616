/**
 * Copyright (C) 2024 GZ-OB. All Rights Reserved
 * File: module_produce_test.c
 * Desc: 产测模块（PCBA 产测协议那套已整体停用，这里只保留活的部分）
 * Ver : V1.00
 * Revision: Ren
 * Date: 2025-01-08
 *
 * 现状：
 *   PCBA 产测工具那套（PCBA_TEST_CMD_* 报文解析 + pcba*TestAck 应答 + 按键表下发比对）
 *   已整体停用：相关函数实现/调用全被注释，现已删除，同时删掉头文件里那批"有声明、
 *   无实现"的接口（避免将来误调用导致链接失败）。
 *   保留的两条活链路：
 *     - bsp_produce_test 的驱动表引用 .init / .loop（由 task_produce_test 调用）；
 *     - produceTestEventRegister_callback 给 BSP 注册回调。
 *   整机测试（按键 258#，第一步按键检测）另有实现：fsm/state/fsm_state_device_test.c，
 *   与本模块无关，不复用这里的结构体。
 */

#include "module_produce_test.h"

#include <string.h>

#include "config.h"
#include "parameter.h"
#include "event.h"

#define OB_LOG_LEVEL OB_LOG_LEVEL_DEBUG
#include "ob_log.h"
#define TAG "module_produce_test"

/***************Variable***************/
static produce_test_handle_t produceTestHandle;
static produce_test_callback_t produceTest_callback;

// ------------------------------------------
static void produceTestHandleEvent_callback(uint8_t result, uint8_t value)
{
    if (NULL != produceTest_callback)
    {
        produceTest_callback(result, value);
    }
#if (Enabled == PRINTF_ERR)
    else
    {
        OB_LOGE(TAG, "Err: produce test' callback is null");
    }
#endif
}

//  @brief 注册回调函数
//  @param callback
void produceTestEventRegister_callback(produce_test_callback_t callback)
{
    produceTest_callback = callback;

#if (Enabled == PRINTF_TEST)
    OB_LOGD(TAG, "produce_test_register_callback:%X", callback);
#endif
}

void produceTestInit(void)
{
    memset((uint8_t *)(&produceTestHandle), 0, sizeof(produce_test_handle_t));
    produceInfoInit();
    produceTestHandleEvent_callback(EVENT_RESULT_PRODUCE_INIT, 0);
}

void produceTestLoop(void)
{
    // 老的 PCBA 产测报文处理（PCBA_TEST_CMD_* 状态机）已停用，这里不再处理任何数据
}
