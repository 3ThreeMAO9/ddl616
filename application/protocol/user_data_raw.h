/**
 * Copyright (c) 2026 GZ-OB, All rights reserved.
 * File name: user_data_raw.h
 * Desc: 用户数据 raw（附表 1）——0x01 应答（门锁->模块）与 0x05/0x07 下发（模块->门锁）共用
 * Version: 1.0.0
 * Revision: James_Zhang
 * Date: 2026-10-14
 *
 * 字节顺序：
 *   p_user_id(1) + p_user_name(41) + timestamp(4) + p_user_policy(1)
 *   + p_user_effective_date(4) + p_user_expire_date(4) + p_user_valid_day(1)
 *   + p_user_effective_time(4) + p_user_expire_time(4)   = 64 字节
 *   + keys_num(1)                                        （钥匙数量 0~13）
 *   + 每把钥匙：p_key_type(1) + p_key_id(1) + p_key_urgent(1) + timestamp(4) = 7 字节
 *
 * 注意：字段顺序和 user_profile_t 不同（policy 在第 4 位），所以按字段逐个拼/解。
 *       下发（0x05/0x07）时 p_user_id = 0xFF 表示由设备端分配用户ID。
 */

#ifndef USER_DATA_RAW_HH
#define USER_DATA_RAW_HH

#include <stdint.h>
#include "user.h"               // user_profile_t / PROFILE_NAME_MAX_LEN

/***********Macro***********/
#define USERDATA_RAW_MAX_KEY        (13)        // keys_num 上限（附表 1：0~13）
#define USERDATA_RAW_USER_ID_AUTO   (0xFF)      // 下发时 p_user_id = 0xFF：由设备端分配用户ID

/***********Struct***********/
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

/* 单条 raw 最大长度：64 + 1 + 13 * 7 = 156 */
#define USERDATA_RAW_MAX_LEN        (sizeof(userdata_raw_head_t) + 1 + USERDATA_RAW_MAX_KEY * sizeof(userdata_raw_key_t))

/* 自检：字节数必须和附表 1 一致（不一致时编译报错） */
typedef char userdata_raw_head_size_check[(sizeof(userdata_raw_head_t) == 64) ? 1 : -1];
typedef char userdata_raw_key_size_check[(sizeof(userdata_raw_key_t) == 7) ? 1 : -1];

/***********Function***********/
/* 打包用户数据 raw（发送方向）：成功返回长度，失败返回 0 */
uint16_t user_data_raw_pack(uint8_t user_id, uint8_t* buf, uint16_t max_len);

/*****************************/

#endif // USER_DATA_RAW_HH
