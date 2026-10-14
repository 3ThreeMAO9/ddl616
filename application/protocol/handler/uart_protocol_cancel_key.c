/**
 * Copyright (c) 2026 GZ-OB, All rights reserved.
 * File name: uart_protocol_cancel_key.c
 * Desc: 取消添加数字钥匙（服务器下发 -> WiFi 模块 -> 门锁）
 * Version: 1.0.0
 * Revision: James_Zhang
 * Date: 2026-10-14
 *
 * 说明：命令字 0x03 / 应答码 0x83，与 参考/ob_lock.h 的
 *       CMD_SERVER_CANCEL_NUMBER_KEY / CMD_LOCK_CANCEL_NUMBER_KEY_RSP 一致。
 *       模块侧下发时 payload 为空（参考/tm_handler/actions_handler.c：
 *       async_send_cmd(CMD_SERVER_CANCEL_NUMBER_KEY, NULL, 0, 5000, ...) ），门锁回 0x83 即可。
 *       云端加密码（0x02）是同步立即完成的，没有挂起的异步录入可取消；
 *       但本地菜单可能正停在"添加密码/指纹/卡片"的录入界面，这里顺手让它退回上级：
 *       推 EVENT_RESULT_CANCEL_ADD_KEY，没有状态处理时 FSM 直接忽略，不影响应答。
 */

#include "uart_protocol.h"
#include "event.h"

#define OB_LOG_LEVEL OB_LOG_LEVEL_DEFAULT
#include "ob_log.h"
#define TAG "up_cancel_key"

/***************Function***************/

// ============================================================
// 协议 handler
// ============================================================
HANDLER_DEFINE(UP_CMD_CANCEL_NUMBER_KEY)
{
    OB_LOGI(TAG, "recv cancel add key (len=%u)", packet->length);

    // 让正在录入的菜单退回上级（不在录入状态时无人处理，事件被忽略）
    baseEventPush(Q_HANDLE_SIG, EVENT_RESULT_CANCEL_ADD_KEY);

    uart_msg_ack(UP_CMD_CANCEL_NUMBER_KEY_ACK, STATUS_SUCCESS);

    return 0;
}
