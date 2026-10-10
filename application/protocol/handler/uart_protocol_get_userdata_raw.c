/**
 * Copyright (c) 2026 GZ-OB, All rights reserved.
 * File name: uart_protocol_get_userdata_raw.c
 * Desc: 获取原始用户数据（服务器下发 -> WiFi 模块 -> 门锁）
 * Version: 1.0.0
 * Revision: James_Zhang
 * Date: 2026-10-10
 *
 * 说明：命令字 0x01 / 应答码 0x81，与 参考/ob_lock.h 的
 *       CMD_SERVER_GET_USERDATA_RAW / CMD_LOCK_GET_USERDATA_RAW_RSP 一致。
 *       下发 payload = kiot_tm_action_in_a_get_user_data_raw_stu_t（1 字节 p_user_id）。
 *       应答 payload = 用户数据 raw（附表 1），按字节顺序：
 *         p_user_id(1) + p_user_name(41) + timestamp(4) + p_user_policy(1)
 *         + p_user_effective_date(4) + p_user_expire_date(4) + p_user_valid_day(1)
 *         + p_user_effective_time(4) + p_user_expire_time(4)   = 64 字节
 *         + keys_num(1)                                        （钥匙数量 0~13）
 *         + 每把钥匙：p_key_type(1) + p_key_id(1) + p_key_urgent(1) + timestamp(4) = 7 字节
 *       注意：附表 1 的字段顺序和 user_profile_t 不同（policy 在第 4 位），所以按字段逐个拼。
 */

#include "uart_protocol.h"
#include "user.h"

#define OB_LOG_LEVEL OB_LOG_LEVEL_DEFAULT
#include "ob_log.h"
#define TAG "up_getdata"

/* keys_num 上限（附表 1：0~13） */
#define USERDATA_RAW_MAX_KEY        (13)
/* 单条应答最大长度：64 + 1 + 13 * 7 = 156 */
#define USERDATA_RAW_MAX_LEN        (sizeof(userdata_raw_head_t) + 1 + USERDATA_RAW_MAX_KEY * sizeof(userdata_raw_key_t))

#pragma pack(1)

/* 附表 1：用户档案部分（64 字节） */
typedef struct
{
    uint8_t  user_id;                           // 1  用户ID
    char     user_name[PROFILE_NAME_MAX_LEN];   // 41 用户名称
    uint32_t timestamp;                         // 4  用户时间戳（最后编辑时间）
    uint8_t  policy;                            // 1  用户策略：0=永久 1=自定义
    uint32_t effective_date;                    // 4  生效日期（unix s，自定义时有效）
    uint32_t expire_date;                       // 4  失效日期（unix s）
    uint8_t  valid_day;                         // 1  生效天（bit0=周日 … bit6=周六）
    uint32_t effective_time;                    // 4  当天生效时间（0~86400s）
    uint32_t expire_time;                       // 4  当天失效时间（0~86400s）
} userdata_raw_head_t;

/* 附表 1：每把数字钥匙（7 字节） */
typedef struct
{
    uint8_t  key_type;                          // 1  1=指纹 2=密码 3=卡片 4=人脸
    uint8_t  key_id;                            // 1  数字钥匙ID
    uint8_t  key_urgent;                        // 1  胁迫标记
    uint32_t timestamp;                         // 4  数字钥匙添加时间
} userdata_raw_key_t;

#pragma pack()

/* 自检：字节数必须和附表 1 一致（不一致时编译报错） */
typedef char userdata_raw_head_size_check[(sizeof(userdata_raw_head_t) == 64) ? 1 : -1];
typedef char userdata_raw_key_size_check[(sizeof(userdata_raw_key_t) == 7) ? 1 : -1];

/***************Function***************/

// ============================================================
// 用户数据 raw 打包（附表 1）：成功返回长度，失败返回 0
// handler 与测试（test/test_file/user_data_raw_test.c）共用
// ============================================================
uint16_t user_data_raw_pack(uint8_t user_id, uint8_t* buf, uint16_t max_len)
{
    userdata_raw_head_t *head;
    userdata_raw_key_t *key_entry;
    user_key_info_t key;
    user_profile_t profile;
    uint8_t key_cnt;
    uint8_t i;

    if ((buf == NULL) || (max_len < (uint16_t)(sizeof(userdata_raw_head_t) + 1)))
    {
        return 0;
    }

    if (!user_get_profile(user_id, &profile))
    {
        return 0;
    }

    // 钥匙数量：不超过 USERDATA_RAW_MAX_KEY，也不超过传入缓冲放得下的数量
    key_cnt = user_get_key_cnt(user_id);
    if (key_cnt > USERDATA_RAW_MAX_KEY)
    {
        key_cnt = USERDATA_RAW_MAX_KEY;
    }
    while ((key_cnt > 0) && ((uint16_t)(sizeof(userdata_raw_head_t) + 1
                                        + key_cnt * sizeof(userdata_raw_key_t)) > max_len))
    {
        key_cnt--;
    }

    memset(buf, 0, (uint16_t)(sizeof(userdata_raw_head_t) + 1 + key_cnt * sizeof(userdata_raw_key_t)));

    // ---- 用户档案部分（64 字节）----
    head = (userdata_raw_head_t *)buf;
    head->user_id = profile.user_id;
    for (i = 0; (i < sizeof(head->user_name)) && profile.user_name[i]; i++)
    {
        head->user_name[i] = profile.user_name[i];
    }
    head->timestamp      = profile.timestamp;
    head->policy         = profile.policy;
    head->effective_date = profile.effective_date;
    head->expire_date    = profile.expire_date;
    head->valid_day      = profile.valid_day;
    head->effective_time = profile.effective_time;
    head->expire_time    = profile.expire_time;

    // ---- keys_num + 每把钥匙（7 字节/把）----
    buf[sizeof(userdata_raw_head_t)] = key_cnt;
    for (i = 0; i < key_cnt; i++)
    {
        if (!user_get_key_info(user_id, i, &key))
        {
            break;
        }

        key_entry = (userdata_raw_key_t *)(buf + sizeof(userdata_raw_head_t) + 1
                                           + i * sizeof(userdata_raw_key_t));
        key_entry->key_type   = key.key_type;
        key_entry->key_id     = key.key_id;
        key_entry->key_urgent = key.key_urgent;
        key_entry->timestamp  = key.timestamp;
    }

    return (uint16_t)(sizeof(userdata_raw_head_t) + 1 + i * sizeof(userdata_raw_key_t));
}

// ============================================================
// 协议 handler
// ============================================================
HANDLER_DEFINE(UP_CMD_GET_USERDATA_RAW)
{
    const kiot_tm_action_in_a_get_user_data_raw_stu_t *req;
    uint8_t buf[USERDATA_RAW_MAX_LEN];
    uint8_t user_id;
    uint16_t len;

    if (packet->length < sizeof(kiot_tm_action_in_a_get_user_data_raw_stu_t))
    {
        OB_LOGW(TAG, "len=%u too short, cmd=0x%02X", packet->length, packet->cmd);
        return 0;
    }

    req = (const kiot_tm_action_in_a_get_user_data_raw_stu_t *)packet->payload;
    user_id = req->p_user_id;

    len = user_data_raw_pack(user_id, buf, sizeof(buf));
    if (len == 0)
    {
        OB_LOGW(TAG, "user %u not exist", user_id);
        uart_msg_ack(UP_CMD_GET_USERDATA_RAW_ACK, STATUS_FAILED);
        return 0;
    }

    OB_LOGI(TAG, "user %u: raw_len=%u", user_id, len);

    uart_msg_userdata_raw(buf, len);

    return 0;
}
