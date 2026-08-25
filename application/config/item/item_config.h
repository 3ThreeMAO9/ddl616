/**
 * Copyright (c) 2025 GZ-OB, All rights reserved.
 * File name: item_config.h
 * Desc:
 * Version: 1.0.0
 * Revision: James_Zhang
 * Date: 2025-12-01
 */

#ifndef _ITEM_CONFIG_H_
#define _ITEM_CONFIG_H_

#include "type_def.h"
#include "base_attribute.h"
/***********Macro***********/
#define NFC_ENABLE                              Enabled

/***********Enum***********/
typedef enum{
    ENTER_SLEEP = 0,
    EXIT_SLEEP,
}sleep_state_e;


/***********Variable***********/

/*************************Struct*************************/
typedef struct
{
    uint16_t client_project;
    uint16_t outside_version;
} device_version_info_t;

/*************************Function*************************/
uint16_t readDeviceClientProjectSn(void);
uint16_t readDeviceVersion(void);
void read_device_version_info(void);

/*************************Project Adaptation*************************/
/**
*******************************************/
#define COFNIG_KDS      (39)
/*******************************************/

/**
*******************************************/
#define PROJECT_140             140
/*******************************************/

/**
*******************************************/
#if (COFNIG_KDS == CONFIG_SEL)

#if (PROJECT_140 == PROJECT_NUMBER)
#include "item_39140.h"
#endif

#endif
/*******************************************/
#include "chip_config.h"

#define DEVICE_TEST_KEY_BOARD_TAB {KEY_NUM_1, KEY_NUM_2, KEY_NUM_3, KEY_NUM_4, KEY_NUM_5, KEY_NUM_6, \
                                   KEY_NUM_7, KEY_NUM_8, KEY_CAN, KEY_NUM_9, KEY_NUM_0, KEY_OK}

///< 0-防撬报警参数
#define TAMPER_ALARM_FLAG_MIN          Disabled
#define TAMPER_ALARM_FLAG_MAX          Enabled
#define TAMPER_ALARM_FLAG_DEFAULT      Enabled

///< 1-逗留报警参数
#define LOITER_ALARM_FLAG_MIN          Disabled
#define LOITER_ALARM_FLAG_MAX          Enabled
#define LOITER_ALARM_FLAG_DEFAULT      Enabled

///< 2-人脸功能设置参数
#define FACE_FUNC_SETTING_MIN          Disabled
#define FACE_FUNC_SETTING_MAX          Enabled
#define FACE_FUNC_SETTING_DEFAULT      Enabled

///< 3-人体感应设置参数
typedef enum {
    OB_LOCK_MOTION_DETECT_SETTINGS_DISABLE,
    OB_LOCK_MOTION_DETECT_SETTINGS_LOW,
    OB_LOCK_MOTION_DETECT_SETTINGS_MID,
    OB_LOCK_MOTION_DETECT_SETTINGS_HIGH,
    OB_LOCK_MOTION_DETECT_SETTINGS_MAX,

} ob_lock_motion_detect_sen_e;

#define HUMAN_SENSOR_SETTING_MIN       (OB_LOCK_MOTION_DETECT_SETTINGS_DISABLE)
#define HUMAN_SENSOR_SETTING_MAX       (OB_LOCK_MOTION_DETECT_SETTINGS_HIGH)
#define HUMAN_SENSOR_SETTING_DEFAULT   (OB_LOCK_MOTION_DETECT_SETTINGS_HIGH)




#endif /* _ITEM_CONFIG_H_ */
