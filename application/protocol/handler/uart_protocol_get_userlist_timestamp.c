/**
 * Copyright (c) 2026 GZ-OB, All rights reserved.
 * File name: uart_protocol_get_userlist_timestamp.c
 * Desc: 获取用户列表时间戳（服务器下发 -> WiFi 模块 -> 门锁）
 * Version: 1.0.0
 * Revision: James_Zhang
 * Date: 2026-10-08
 *
 * 说明：命令字 0x08 / 应答码 0x88。应答 payload = uint32 数组，固定 50 项，
 *       下标即用户ID，值为该用户最后修改时间，0 表示用户不存在。
 */

#include "uart_protocol.h"
#include "user.h"

#define OB_LOG_LEVEL OB_LOG_LEVEL_DEFAULT
#include "ob_log.h"
#define TAG "up_get_user_ts"

// 时间戳项数：下标即用户ID
#define USERLIST_TIMESTAMP_NUM      (PROFILE_COUNT)

/***************Variable***************/


/***************Function***************/

// ============================================================
// 协议 handler
// ============================================================
HANDLER_DEFINE(UP_CMD_GET_USERLIST_TIMESTAMP)
{
    uint8_t count = USERLIST_TIMESTAMP_NUM;
    uint32_t s_userlist_timestamp[USERLIST_TIMESTAMP_NUM];

    user_get_list_timestamp(s_userlist_timestamp, count);

    OB_LOGI(TAG, "recv get userlist timestamp, cmd=0x%02X, len=%u, count=%u",
            packet->cmd, packet->length, count);

    uart_msg_userlist_timestamp(s_userlist_timestamp, count);

    return 0;
}
