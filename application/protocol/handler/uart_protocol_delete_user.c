/**
 * Copyright (c) 2026 GZ-OB, All rights reserved.
 * File name: uart_protocol_delete_user.c
 * Desc: 删除用户（服务器下发 -> WiFi 模块 -> 门锁）
 * Version: 1.1.0
 * Revision: James_Zhang
 * Date: 2026-10-14
 *
 * 说明：命令字 0x06 / 应答码 0x86，与 参考/ob_lock.h 的
 *       CMD_SERVER_DELETE_USER / CMD_LOCK_DELETE_USER_RSP 一致。
 *       下发 payload = kiot_tm_action_in_a_del_user_stu_t（1 字节 p_user_id）。
 *       语义：删掉该用户名下的**全部钥匙 + 档案**（user_delete_normal 内部处理，
 *             指纹会同时把模块里的模板入队删掉）；0 = 管理员不可删；用户不存在 -> 失败。
 *       提示：本命令的 in 本来就是 1 字节，所以不受 WiFi 模块侧 raw 长度 bug 影响，可直接联调。
 *
 *       应答规则（kiot action 是透传，不带 status）：本命令 out 无 payload ->
 *         成功：回空 payload（uart_msg_empty_ack）
 *         失败：暂不回应答（待与模块侧确认失败如何表达）
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

    OB_LOGI(TAG, "delete user OK: id=%u", user_id);

    uart_msg_empty_ack(UP_CMD_DELETE_USER_ACK);             // 成功：out 无 payload

    return 0;
}
