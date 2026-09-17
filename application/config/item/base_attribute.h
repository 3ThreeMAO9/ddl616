/**
 * Copyright (c) 2025 GZ-OB, All rights reserved.
 * File name: base_attribute.h
 * Desc:
 * Version: 1.0.0
 * Revision: James_Zhang
 * Date: 2025-12-01
 */

#ifndef BASE_ATTRIBUTE_H
#define BASE_ATTRIBUTE_H

#include "type_def.h"


/***********Macro***********/
//client and item sn
#define CLIENT_ITEM_SN_39140        (39140)// 39-140

// 20XX(编译年份)-01-01
#define SYSTEM_TIME_YEAR_DEFAULT    ((__DATE__[9] - '0') * 10 + (__DATE__[10] - '0'))
#define SYSTEM_TIME_MONTH_DEFAULT   1
#define SYSTEM_TIME_DAY_DEFAULT     1

// 00:00:00
#define SYSTEM_TIME_HOUR_DEFAULT    0
#define SYSTEM_TIME_MINUTE_DEFAULT  0
#define SYSTEM_TIME_SECOND_DEFAULT  0

/***********Enum***********/
/* ============================================================
 * 用户参数 ID 定义
 * 分块规则：
 *   0x00 ~ 0x1F: 锁体业务参数
 *   0x20 ~ 0x2F: BLE 参数
 * 注意：新增参数必须放入对应模块的预留区内，
 *       严禁在中间插入，否则会导致 Flash 参数错位！
 * ============================================================ */
typedef enum
{
    /* ---------- 锁体业务参数 (0x00 ~ 0x1F) ---------- */
    USER_PARA_VERIFY_MODE_ID       = 0x00,  // 验证方式（单一/双重/常开）
    USER_PARA_AUTO_LOCK_MODE_ID    = 0x01,  // 自动上锁模式
    USER_PARA_SILENT_MODE_ID       = 0x02,  // 静音模式
    USER_PARA_VACATION_MODE_ID     = 0x03,  // 假期模式
    USER_PARA_MOTOR_DIRECTION_ID   = 0x04,  // 电机方向
    USER_PARA_SYSTEM_LOCK_ID       = 0x05,  // 系统锁定状态
    USER_PARA_VERIFY_FAIL_CNT_ID   = 0x06,  // 验证失败累计次数
    USER_PARA_BREAK_ID             = 0x07,  // 防拆报警
    USER_PARA_LANGUAGE_MODE_ID     = 0x08,  // 语言模式
    USER_PARA_VACATION_WARM_ID     = 0x09,  // 假期模式
    USER_PARA_LOCKED_ROTOR_WARM_ID = 0x0A,  // 堵转/锁死

    /* ---------- BLE 参数 (0x20 ~ 0x2F) ---------- */
    BLE_ACTIVATION_FLAG_ID         = 0x20,  // BLE 激活标志
    BLE_NET_STATUS_FLAG_ID         = 0x21,  // BLE 网络状态
    BLE_RESET_STATUS_ID            = 0x22,  // BLE 复位状态
    BLE_BIND_FLAG_ID               = 0x23,  // BLE 绑定标志
    BLE_TIME_ZONE_ID               = 0x24,  // BLE 时区
    BLE_RESERVED_ID                = 0x2F,

    USER_PARA_CNT
} user_para_id_t;

typedef enum{
    DIRECTION_LEFT = 0,
    DIRECTION_RIGHT,
    
}motor_direction_t;

typedef enum{
    TRUN_OFF = 0,       //键盘灯全灭
    TRUN_ON,            //键盘灯全亮
    KEY_OFF,            //按键熄灭对应键盘灯
    KEY_ON,             //按键亮起对应键盘灯
    OPTIONAL_ON,        //可选键盘灯
}swtich_t;

/***********Struct***********/

/***********Variable***********/

/***********Function***********/

/*****************************/

#endif /* BASE_ATTRIBUTE_H */


