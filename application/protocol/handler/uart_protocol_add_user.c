/**
 * Copyright (c) 2026 GZ-OB, All rights reserved.
 * File name: uart_protocol_add_user.c
 * Desc: 添加用户（服务器下发 -> WiFi 模块 -> 门锁）
 * Version: 1.1.0
 * Revision: James_Zhang
 * Date: 2026-10-14
 *
 * 说明：命令字 0x05 / 应答码 0x85，与 参考/ob_lock.h 的
 *       CMD_SERVER_ADD_USER / CMD_LOCK_ADD_USER_RSP 一致。
 *       下发 payload = 用户数据 raw（见 user_data_raw.h / 附表 1）；
 *       其中 p_user_id = 0xFF 表示由设备端分配用户ID（取最小空闲档案ID）。
 *       应答 payload = 1 字节用户ID（kiot_tm_action_out_a_add_user_raw_stu_t）。
 *       钥匙清单（keys_num + 每把 7 字节）只解析打印、不落库：raw 里只有元数据
 *       （无密码明文、无指纹模板），钥匙实体由 0x02 添加数字钥匙 / 本地录入产生。
 */

#include "uart_protocol.h"
#include "user.h"
#include "user_data_raw.h"

#define OB_LOG_LEVEL OB_LOG_LEVEL_DEFAULT
#include "ob_log.h"
#define TAG "up_add_user"

/***************Function***************/

// 解析 raw -> 写档案。成功返回用户ID，失败返回 0
static uint16_t add_user_apply(const uart_packet_t* packet)
{
    userdata_raw_head_t head;
    user_profile_t profile;
    uint16_t user_id;

    // 1. 解 raw（payload 未对齐，必须 memcpy 到本地结构体）
    memcpy(&head, packet->payload, sizeof(head));

    // 2. 用户ID：0xFF = 由设备端分配（最小空闲档案ID，返回 0 表示已满）
    //    用 find_free_profile_id 而不是 alloc_user_id：后者会刷新本地录入的"即将创建用户ID"缓存
    user_id = head.user_id;
    if (USERDATA_RAW_USER_ID_AUTO == user_id)
    {
        user_id = find_free_profile_id();
        if (0 == user_id)
        {
            OB_LOGE(TAG, "no free user id");
            return 0;
        }
    }
    else if ((0 == user_id) || (user_id >= PROFILE_COUNT))
    {
        OB_LOGW(TAG, "invalid user id %u", user_id);
        return 0;
    }

    // 3. 组档案（附表 1 的字段顺序和 user_profile_t 不同，逐个搬）
    memset(&profile, 0, sizeof(profile));
    memcpy(profile.user_name, head.user_name, PROFILE_NAME_MAX_LEN);
    profile.user_name[PROFILE_NAME_MAX_LEN - 1] = '\0';     // 名字定长 41，保证有结束符
    profile.policy         = head.policy;
    profile.timestamp      = head.timestamp;                // 存服务器给的时间，不刷成本地时间
    profile.effective_date = head.effective_date;
    profile.expire_date    = head.expire_date;
    profile.valid_day      = head.valid_day;
    profile.effective_time = head.effective_time;
    profile.expire_time    = head.expire_time;

    if (!user_profile_write(user_id, &profile))
    {
        OB_LOGE(TAG, "write profile fail, id=%u", user_id);
        return 0;
    }

    return user_id;
}

// 打印 raw 里的钥匙清单（只校验长度并记日志）
static void add_user_dump_keys(const uart_packet_t* packet)
{
    userdata_raw_key_t key;
    uint8_t keys_num = packet->payload[sizeof(userdata_raw_head_t)];
    uint16_t offset  = sizeof(userdata_raw_head_t) + 1;
    uint8_t i;

    for (i = 0; i < keys_num; i++)
    {
        if ((uint16_t)(offset + sizeof(key)) > packet->length)
        {
            OB_LOGW(TAG, "keys_num=%u but len=%u, cut at %u", keys_num, packet->length, i);
            break;
        }

        memcpy(&key, packet->payload + offset, sizeof(key));
        OB_LOGI(TAG, "  key[%u]: type=%u, id=%u, urgent=%u, ts=%u",
                i, key.key_type, key.key_id, key.key_urgent, key.timestamp);
        offset += sizeof(key);
    }
}

// ============================================================
// 协议 handler
// ============================================================
HANDLER_DEFINE(UP_CMD_ADD_USER)
{
    uint16_t user_id;

    // 至少要有档案部分(64) + keys_num(1)
    if (packet->length < (sizeof(userdata_raw_head_t) + 1))
    {
        OB_LOGW(TAG, "recv add user, len=%u too short (need >= %u)",
                packet->length, (uint16_t)(sizeof(userdata_raw_head_t) + 1));
        uart_msg_ack(UP_CMD_ADD_USER_ACK, STATUS_FAILED);
        return 0;
    }

    OB_LOGI(TAG, "recv add user: req_id=0x%02X, keys_num=%u, len=%u",
            packet->payload[0], packet->payload[sizeof(userdata_raw_head_t)], packet->length);

    add_user_dump_keys(packet);

    user_id = add_user_apply(packet);
    if (0 == user_id)
    {
        uart_msg_ack(UP_CMD_ADD_USER_ACK, STATUS_FAILED);
        return 0;
    }

    OB_LOGI(TAG, "add user OK: id=%u", user_id);

    uart_msg_add_user_ack((uint8_t)user_id);

    return 0;
}
