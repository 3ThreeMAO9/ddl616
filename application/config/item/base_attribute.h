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
#define SYSTEM_TIME_YEAR_DEFAULT    ((__DATE__[9] - '0')*10+(__DATE__[10] - '0'))
#define SYSTEM_TIME_MONTH_DEFAULT   1
#define SYSTEM_TIME_DAY_DEFAULT     1

// 00:00:00
#define SYSTEM_TIME_HOUR_DEFAULT    0
#define SYSTEM_TIME_MINUTE_DEFAULT  0
#define SYSTEM_TIME_SECOND_DEFAULT  0

/***********Enum***********/
// user parameter
typedef enum{
    USER_PARA_AUTO_LOCK_TIME_ID = 0,
    USER_PARA_AUTO_LOCK_MODE_ID,
    USER_PARA_SILENT_MODE_ID,
    USER_PARA_VACATION_MODE_ID,
    USER_PARA_MOTOR_DIRECTION_ID,
    USER_PARA_SYSTEM_LOCK_ID,
    USER_PARA_VERIFY_FAIL_CNT_ID,
    USER_PARA_INIT_MOTOR_RESET_ID,
    USER_PARA_LANGUAGE_MODE_ID,

    BLE_ACTIVATION_FLAG_ID,
    BLE_NET_STATUS_FLAG_ID,
    BLE_RESET_STATUS_ID,
    BLE_BIND_FLAG_ID,
    BLE_TIME_ZONE_ID,

    USER_PARA_VACATION_WARM_ID,
    USER_PARA_LOCKED_ROTOR_WARM_ID,

    USER_PARA_CNT,

}user_para_id_t;

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


