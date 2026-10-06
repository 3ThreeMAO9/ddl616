/**
 * Copyright (c) 2026 GZ-OB, All rights reserved.
 * File name: uart_protocol_delete_user.c
 * Desc: 删除用户（服务器下发 -> WiFi 模块 -> 门锁）
 * Version: 1.0.0
 * Revision: James_Zhang
 * Date: 2026-10-05
 *
 * 说明：本文件目前只实现接口骨架。
 *       服务器下发的报文结构尚未确定，因此不定义 payload 结构体、不解析、不应答。
 *       命令字 0x06 / 应答码 0x86 与 ob_lock.h 的
 *       CMD_SERVER_DELETE_USER / CMD_LOCK_DELETE_USER_RSP 一致。
 */

#include "uart_protocol.h"

#define OB_LOG_LEVEL OB_LOG_LEVEL_DEFAULT
#include "ob_log.h"
#define TAG "up_del_user"

/***************Function***************/

// ============================================================
// 协议 handler
// ============================================================
HANDLER_DEFINE(UP_CMD_DELETE_USER)
{
    // TODO: 服务器下发的 payload 结构尚未确定，暂不解析、不应答。
    //       待格式确认后补：定义 payload 结构体 -> 解析参数 -> 删除用户
    //       -> 用 UP_CMD_DELETE_USER_ACK（0x86）回送应答。
    OB_LOGI(TAG, "recv delete user, cmd=0x%02X, len=%u (interface only)",
            packet->cmd, packet->length);

    return 0;
}
