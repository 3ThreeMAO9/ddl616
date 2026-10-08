/**
 * Copyright (c) 2026 GZ-OB, All rights reserved.
 * File name: uart_protocol_get_userlist_timestamp.c
 * Desc: 获取用户列表时间戳（服务器下发 -> WiFi 模块 -> 门锁）
 * Version: 1.0.0
 * Revision: James_Zhang
 * Date: 2026-10-08
 *
 * 说明：命令字 0x08 / 应答码 0x88 与 ob_lock.h 的
 *       CMD_SERVER_GET_USERLIST_TIMESTAMP / CMD_LOCK_GET_USERLIST_TIMESTAMP_RSP 一致。
 *       服务器下发不带 payload。
 *       应答 payload = uint32 数组（每项 4 字节小端），对应物模型 p_user_list_timestamp
 *       （array，maxLength 50，item uint32）。语义由 WiFi 模块研发确认：
 *       固定 50 项，**下标即用户ID**，值 = 该用户最后修改时间（timestamp），
 *       **0 表示该用户不存在**。
 */

#include "uart_protocol.h"
#include "user.h"

#define OB_LOG_LEVEL OB_LOG_LEVEL_DEFAULT
#include "ob_log.h"
#define TAG "up_get_user_ts"

/* 用户表时间戳项数：设备本地 50 个用户，下标即用户ID（0~49） */
#define USERLIST_TIMESTAMP_NUM      (PROFILE_COUNT)

/***************Variable***************/
/* 用户表时间戳数组：下标=用户ID，值为该用户最后修改时间，0 表示用户不存在 */
static uint32_t s_userlist_timestamp[USERLIST_TIMESTAMP_NUM];

/***************Function***************/

// ============================================================
// 协议 handler
// ============================================================
HANDLER_DEFINE(UP_CMD_GET_USERLIST_TIMESTAMP)
{
    uint8_t count = USERLIST_TIMESTAMP_NUM;

    // 读取本地用户表时间戳：下标=用户ID，不存在的用户为 0
    user_get_list_timestamp(s_userlist_timestamp, count);

    OB_LOGI(TAG, "recv get userlist timestamp, cmd=0x%02X, len=%u, count=%u",
            packet->cmd, packet->length, count);

    // 回送时间戳数组（0x88）
    uart_msg_userlist_timestamp(s_userlist_timestamp, count);

    return 0;
}
