/**
 * Copyright (c) 2026 GZ-OB, All rights reserved.
 * File name: uart_protocol_cancel_key.c
 * Desc: 取消添加数字钥匙（服务器下发 -> WiFi 模块 -> 门锁）
 * Version: 1.1.0
 * Revision: James_Zhang
 * Date: 2026-10-14
 *
 * 说明：命令字 0x03 / 应答码 0x83。payload 为空。云端加密码(0x02)是同步完成的、没有挂起的
 *       异步录入可取消；但本地菜单可能正停在"添加密码/指纹/卡片"录入界面，这里推
 *       EVENT_RESULT_CANCEL_ADD_KEY 让它退回上级（无状态处理时 FSM 忽略）。
 *       应答：成功回空 payload。
 */

#include "uart_protocol.h"
#include "event.h"

#define OB_LOG_LEVEL OB_LOG_LEVEL_NONE
#include "ob_log.h"
#define TAG "up_cancel_key"

/***************Function***************/

// ============================================================
// 协议 handler
// ============================================================
HANDLER_DEFINE(UP_CMD_CANCEL_NUMBER_KEY)
{

    // 让正在录入的菜单退回上级（不在录入状态时无人处理，事件被忽略）
    baseEventPush(Q_HANDLE_SIG, EVENT_RESULT_CANCEL_ADD_KEY);

    uart_msg_empty_ack(packet->cmd);       // 成功：out 无 payload

    return 0;
}
