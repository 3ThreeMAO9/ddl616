/**
 * Copyright (c) 2026 GZ-OB, All rights reserved.
 * File name: uart_protocol_add_user.c
 * Desc: 添加用户（服务器下发 -> WiFi 模块 -> 门锁）
 * Version: 1.5.0
 * Revision: James_Zhang
 * Date: 2026-10-14
 *
 * 说明：命令字 0x05 / 应答码 0x85，与 参考/ob_lock.h 的
 *       CMD_SERVER_ADD_USER / CMD_LOCK_ADD_USER_RSP 一致。
 *       下发 payload **只有 1 字节 p_user_id**，APP 约定固定送 0xFF：
 *         p_user_id = 0xFF -> 由设备端确定"当前可用的用户ID"
 *                             （0~49 都算，含管理用户：空锁上就分到 0）
 *       本命令**不带档案字段**（名字/策略/有效期…），档案随后由 0x07 set_user 下发，
 *       这里先按默认档案建出来（永久有效；管理用户兜底昵称 "Admin"）。
 *
 *       应答规则（kiot action 是透传，不带 status）：本命令 out = 1 字节用户ID ->
 *         成功：回真实用户ID（0 是合法值 = 管理用户）
 *         失败：暂不回应答（待与模块侧确认；0 不能兼作失败标志，否则和"管理用户"撞车）
 */

#include "uart_protocol.h"
#include "user.h"
#include "user_data_raw.h"      // USERDATA_RAW_USER_ID_AUTO
#include "hal_rtc.h"

#define OB_LOG_LEVEL OB_LOG_LEVEL_DEFAULT
#include "ob_log.h"
#define TAG "up_add_user"

#define ADD_USER_NAME_ADMIN     "Admin"     // 没给名字时，管理用户的兜底昵称

/***************Function***************/

// 默认档案：永久有效（普通用户名字留空等 0x07 补；管理用户兜底 "Admin"）
static void add_user_default_profile(user_profile_t* profile, uint8_t user_id)
{
    memset(profile, 0, sizeof(user_profile_t));

    profile->policy         = USER_POLICY_PERMANENT;
    profile->timestamp      = hal_get_rtc_time();
    profile->effective_date = 0xFFFFFFFF;
    profile->expire_date    = 0xFFFFFFFF;
    profile->valid_day      = 0xFF;
    profile->effective_time = 0xFFFFFFFF;
    profile->expire_time    = 0xFFFFFFFF;

    if (0 == user_id)
    {
        memcpy(profile->user_name, ADD_USER_NAME_ADMIN, sizeof(ADD_USER_NAME_ADMIN));
    }
}

// ============================================================
// 协议 handler
// ============================================================
HANDLER_DEFINE(UP_CMD_ADD_USER)
{
    user_profile_t profile;
    uint8_t user_id;

    // 1. 长度校验：只有 1 字节 p_user_id
    if (packet->length < 1)
    {
        OB_LOGW(TAG, "recv add user: empty payload");
        return 0;       // 失败：暂不回应答
    }

    // 2. 0xFF = 由设备端确定一个可用ID（**0~49，含管理用户**）
    //    其它值按"指定普通用户"处理；用 find_free_user_id 而不是 alloc_user_id：
    //    后者会刷新本地录入的"即将创建用户ID"缓存
    user_id = packet->payload[0];
    if (USERDATA_RAW_USER_ID_AUTO == user_id)
    {
        uint16_t free_id = find_free_user_id();

        if (0xFFFF == free_id)
        {
            OB_LOGE(TAG, "no free user id");
            return 0;   // 失败：暂不回应答
        }

        user_id = (uint8_t)free_id;
    }
    else if (user_id >= PROFILE_COUNT)
    {
        OB_LOGW(TAG, "invalid user id %u", user_id);
        return 0;       // 失败：暂不回应答
    }

    // 3. 先按默认档案建出来（字段由 0x07 set_user 补）
    add_user_default_profile(&profile, user_id);

    if (!user_profile_write(user_id, &profile))
    {
        OB_LOGE(TAG, "write profile fail, id=%u", user_id);
        return 0;       // 失败：暂不回应答
    }

    OB_LOGI(TAG, "add user OK: id=%u", user_id);

    uart_msg_add_user_ack(user_id);         // 成功：out = 1 字节用户ID

    return 0;
}
