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
#define CLIENT_ITEM_SN_12345        (12345) //12-345
#define CLIENT_ITEM_SN_22222        (22222) //22-222

// 20XX(编译年份)-01-01
#define SYSTEM_TIME_YEAR_DEFAULT    ((__DATE__[9] - '0')*10+(__DATE__[10] - '0'))
#define SYSTEM_TIME_MONTH_DEFAULT   1
#define SYSTEM_TIME_DAY_DEFAULT     1

// 00:00:00
#define SYSTEM_TIME_HOUR_DEFAULT    0
#define SYSTEM_TIME_MINUTE_DEFAULT  0
#define SYSTEM_TIME_SECOND_DEFAULT  0

/***********Enum***********/
typedef enum
{
    TRUN_OFF = 0,
    TRUN_ON,
} turn_state_t;

/***********Struct***********/

/***********Variable***********/

/***********Function***********/

/*****************************/

#endif /* BASE_ATTRIBUTE_H */


