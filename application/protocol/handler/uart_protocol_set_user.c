/**
 * Copyright (c) 2026 GZ-OB, All rights reserved.
 * File name: uart_protocol_set_user.c
 * Desc: 设置（修改）用户（服务器下发 -> WiFi 模块 -> 门锁）
 * Version: 1.1.0
 * Revision: James_Zhang
 * Date: 2026-10-14
 *
 * 说明：命令字 0x07 / 应答码 0x87，与 参考/ob_lock.h 的
 *       CMD_SERVER_SET_USER / CMD_LOCK_SET_USER_RSP 一致。
 *       下发 payload = 用户数据 raw（同 0x05，见 user_data_raw.h），第 1 字节是 p_user_id
 *       （0 = 管理用户，1~49 = 普通用户；这里 0xFF 不是"请设备分配"，设置必须指定具体用户）。
 *       语义：
 *         raw 完整（>= 65 字节）-> 按附表 1 的字段更新该用户档案（名字/策略/有效期…，
 *                                  timestamp 存服务器给的值，不刷成本地时间）；
 *         raw 不完整（WiFi 模块侧只透传了 p_user_id）-> 只确认该用户存在，**不改任何字段**
 *                                 （不能拿默认档案把已有的名字/有效期抹掉）。
 *       用户不存在 -> 失败。
 *
 *       应答规则（kiot action 是透传，不带 status）：本命令 out 无 payload ->
 *         成功：回空 payload（uart_msg_empty_ack）
 *         失败：暂不回应答（待与模块侧确认失败如何表达）
 */

#include "uart_protocol.h"
#include "user.h"
#include "user_data_raw.h"

#define OB_LOG_LEVEL OB_LOG_LEVEL_DEFAULT
#include "ob_log.h"
#define TAG "up_set_user"

/***************Function***************/

// raw 完整时按附表 1 逐字段搬（字段顺序和 user_profile_t 不同）
static void set_user_raw_profile(const uart_packet_t* packet, user_profile_t* profile)
{
    userdata_raw_head_t head;

    // payload 未对齐，必须 memcpy 到本地结构体
    memcpy(&head, packet->payload, sizeof(head));

    memset(profile, 0, sizeof(user_profile_t));
    memcpy(profile->user_name, head.user_name, PROFILE_NAME_MAX_LEN);
    profile->user_name[PROFILE_NAME_MAX_LEN - 1] = '\0';    // 名字定长 41，保证有结束符

    profile->policy         = head.policy;
    profile->timestamp      = head.timestamp;               // 存服务器给的时间，不刷成本地时间
    profile->effective_date = head.effective_date;
    profile->expire_date    = head.expire_date;
    profile->valid_day      = head.valid_day;
    profile->effective_time = head.effective_time;
    profile->expire_time    = head.expire_time;
}

// ============================================================
// 协议 handler
// ============================================================
HANDLER_DEFINE(UP_CMD_SET_USER)
{
    user_profile_t profile;
    uint8_t user_id;
    uint16_t raw_min_len = sizeof(userdata_raw_head_t) + 1;     // 档案部分 + keys_num

    // 0. 至少要能取到 p_user_id
    if (packet->length < 1)
    {
        OB_LOGW(TAG, "recv set user: empty payload");
        return 0;       // 失败：暂不回应答
    }

    // 1. 必须是已存在的用户（0 = 管理用户也合法）
    user_id = packet->payload[0];
    if (!user_get_profile(user_id, &profile))
    {
        OB_LOGW(TAG, "set user: id=%u not exist", user_id);
        return 0;       // 失败：暂不回应答
    }

    // 2. raw 完整 -> 按字段更新；不完整 -> 只确认存在，不改档案
    if (packet->length >= raw_min_len)
    {
        set_user_raw_profile(packet, &profile);

        if (!user_profile_write(user_id, &profile))
        {
            OB_LOGE(TAG, "set user: write fail, id=%u", user_id);
            return 0;   // 失败：暂不回应答
        }

        OB_LOGI(TAG, "set user OK: id=%u, name=%s, len=%u", user_id, profile.user_name, packet->length);
    }
    else
    {
        OB_LOGW(TAG, "set user: raw incomplete (len=%u < %u), only check exist",
                packet->length, raw_min_len);
        OB_LOGI(TAG, "set user OK: id=%u (no change)", user_id);
    }

    uart_msg_empty_ack(UP_CMD_SET_USER_ACK);                // 成功：out 无 payload

    return 0;
}
