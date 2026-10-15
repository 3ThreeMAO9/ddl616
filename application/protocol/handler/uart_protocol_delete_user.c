/**
 * Copyright (c) 2026 GZ-OB, All rights reserved.
 * File name: uart_protocol_delete_user.c
 * Desc: 删除用户（服务器下发 -> WiFi 模块 -> 门锁）
 * Version: 1.1.0
 * Revision: James_Zhang
 * Date: 2026-10-14
 *
 * 说明：命令字 0x06 / 应答码 0x86。payload = kiot_tm_action_in_a_del_user_stu_t（1 字节 p_user_id）。
 *       删该用户名下全部钥匙 + 档案（user_delete_normal 内部处理，指纹模板同步入队删）；
 *       0=管理员不可删；用户不存在 -> 失败。
 *       应答：成功回空 payload，失败暂不回应答。
 */

#include "uart_protocol.h"
#include "user.h"

#define OB_LOG_LEVEL OB_LOG_LEVEL_DEFAULT
#include "ob_log.h"
#define TAG "up_del_user"

/***************Function***************/

// ============================================================
// 协议 handler
// ============================================================
HANDLER_DEFINE(UP_CMD_DELETE_USER)
{
    user_del_result_t result;
    uint8_t user_id;

    // 1. 长度校验（payload = 1 字节 p_user_id）
    if (packet->length < sizeof(uint8_t))
    {
        OB_LOGW(TAG, "recv delete user: empty payload");
        return 0;       // 失败：暂不回应答
    }

    // 2. 删档案 + 名下全部钥匙
    user_id = packet->payload[0];
    result = user_delete_normal(user_id);

    if (USER_DEL_OK != result)
    {
        // 1=管理员不可删 2=用户不存在 3=没有普通用户
        OB_LOGW(TAG, "delete user fail: id=%u, result=%u", user_id, (uint8_t)result);
        return 0;       // 失败：暂不回应答
    }


    uart_msg_empty_ack(UP_CMD_DELETE_USER_ACK);             // 成功：out 无 payload

    return 0;
}
