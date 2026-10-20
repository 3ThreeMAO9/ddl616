/**
 * Copyright (c) 2026 GZ-OB, All rights reserved.
 * File name: uart_protocol_add_user.c
 * Desc: 添加用户（服务器下发 -> WiFi 模块 -> 门锁）
 * Version: 1.5.0
 * Revision: James_Zhang
 * Date: 2026-10-14
 *
 * 说明：命令字 0x05 / 应答码 0x85。payload 只有 1 字节 p_user_id，APP 固定送 0xFF：
 *       0xFF -> 由设备端分配当前可用用户ID（0~49 含管理用户，空锁上分到 0）。
 *       本命令不带档案字段，先按默认档案建出（永久有效，管理用户昵称兜底 "Admin"），
 *       档案随后由 0x07 set_user 下发。
 *       应答：out = 1 字节用户ID（0 是合法值=管理用户，故不能用 0 表示失败）；失败暂不回应答。
 */

#include "uart_protocol.h"
#include "user.h"
#include "user_data_raw.h"      // USERDATA_RAW_USER_ID_AUTO
#include "hal_rtc.h"

#define OB_LOG_LEVEL OB_LOG_LEVEL_NONE
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
            OB_LOGW(TAG, "no free user id");
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
        OB_LOGW(TAG, "write profile fail, id=%u", user_id);
        return 0;       // 失败：暂不回应答
    }


    uart_msg_add_user_ack(user_id);         // 成功：out = 1 字节用户ID

    return 0;
}
