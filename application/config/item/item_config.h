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

// user config
#define KEY_CNT_MAX                                 (300)
#define USER_CNT_MAX                                (KEY_CNT_MAX)   // user cnt
#define KEY_CNT_EVERY_USER                          (1)

#define USER_CODE_LEN_MIN                           (6)
#define USER_CODE_LEN_MAX                           (12)
#define USER_CODE_LEN_SIZE                          (12)
// #define USER_CODE_LEN_SIZE                          (USER_CODE_LEN_MAX / 2)
#define DUMMY_USER_CODE_LEN_MAX                     (20)

#define LINKED_CODE_LEN_MAX                         (4)

#define ADMIN_PASSWORD_DEFAULT                      {1,2,3,4,5,6,7,8}

//fingerprint parameter
#define FINGER_PRESS_CNT                            (8)
#define FIGNER_ALLOW_REPEAT                         (Enabled)
#define FIGNER_MODULE_CHECK                         (Disabled)

//verify fail' cnt
#define VERIFY_FAIL_CNT_FOR_WARN                    (3)
#define VERIFY_FAIL_CNT_FOR_SYSTEM_LOCK             (5)
#define VERIFY_FAIL_CNT_TIMEOUT                     (5 * 60)        // unit:s

//time out
#define SYSTEM_LOCK_TIME                            (100 * 1000)    // unit:ms
#define SYSTEM_LOCK_TIMEOUT                         (10 * 1000)     // unit:ms
#define WORK_TIMEOUT                                (10000)         // unit:ms
#define WAKE_TIMEOUT                                (1500)          // unit:ms

//log cnt
#define LOG_CNT                                     (200)

//user cnt
#define MASTER_USER_CODE_CNT                        (1)
#define COMMON_USER_CODE_CNT      		            (19)
#define PERMANENT_USER_CODE_CNT                     (MASTER_USER_CODE_CNT + COMMON_USER_CODE_CNT)

#define USER_FINGERPRINTS_CNT                       (50)

#define USER_CARD_CNT                               (100)

#define USER_FACE_CNT                               (50)

#define PERMANENT_USER_CNT                          (PERMANENT_USER_CODE_CNT + USER_FINGERPRINTS_CNT + USER_CARD_CNT + USER_FACE_CNT)

#define USER_CNT                                    (PERMANENT_USER_CNT)

//block' size
#define USER_BLOCK_SIZE                             (32)
#define PARAMETER_BLOCK_SIZE                        (4)
#define DATA_BLOCK_SIZE                             (MAX(USER_BLOCK_SIZE, PARAMETER_BLOCK_SIZE))

//auto lock' time
#define AUTO_LOCK_TIME_MAX                          (180)        //unit: s
#define AUTO_LOCK_TIME_MIN                          (10)        //unit: s
#define AUTO_LOCK_TIME_DEFAULT                      (30)        //unit: s

//auto lock mode
#define AUTO_LOCK_MODE_MIN                          Disabled
#define AUTO_LOCK_MODE_MAX                          Enabled
#define AUTO_LOCK_MODE_DEFAULT                      Disabled

//silent mode
#define SILENT_MODE_MIN                             Disabled
#define SILENT_MODE_MAX                             Enabled
#define SILENT_MODE_DEFAULT                         Enabled

//vacation mode
#define VACATION_MODE_MIN                           Disabled
#define VACATION_MODE_MAX                           Enabled
#define VACATION_MODE_DEFAULT                       Disabled

//motor directin
#define MOTOR_DIRECTION_MIN                         DIRECTION_LEFT
#define MOTOR_DIRECTION_MAX                         DIRECTION_RIGHT
#define MOTOR_DIRECTION_DEFAULT                     DIRECTION_LEFT

//language mode
typedef enum{
    LANGUAGE_CN = 0,
    LANGUAGE_EN,
    LANGUAGE_SP,
    LANGUAGE_FR,
}language_set_t;

#define LANGUAGE_MODE_MIN                           LANGUAGE_CN
#define LANGUAGE_MODE_MAX                           LANGUAGE_FR
#define LANGUAGE_MODE_DEFAULT                       LANGUAGE_CN

//system lock flag
#define SYSTEM_LOCK_FLAG_MIN                        Disabled
#define SYSTEM_LOCK_FLAG_MAX                        Enabled
#define SYSTEM_LOCK_FLAG_DEFAULT                    Disabled

//break flag(init)
#define INIT_BREAK_MIN                              Disabled
#define INIT_BREAK_MAX                              Enabled
#define INIT_BREAK_DEFAULT                          Disabled

//verify fail cnt
#define VERIFY_FAIL_CNT_MIN                          0
#define VERIFY_FAIL_CNT_MAX                          VERIFY_FAIL_CNT_FOR_SYSTEM_LOCK
#define VERIFY_FAIL_CNT_DEFAULT                      0

#define WORK_TIME_OUT_VAULE                         (10 * 1000)     // unit: ms
#define WORK_WAKE_TIME_OUT_VAULE                    (1500)          // unit: ms
#define WORK_TIME_OUT_MAX_VAULE                     (30000)         // unit: ms
#define WORK_TIME_OUT_VAULE_LOWBATTERY              (4000)         // unit: ms
#define HANDLE_RESULT_TIME_OUT                      (1500+100)      // unit: ms
#define WAKE_PAUSE_KEY_TIME                         (50)            //unit： ms

//block' size
#define USER_BLOCK_SIZE                             (32)
#define PARAMETER_BLOCK_SIZE                        (4)
#define DATA_BLOCK_SIZE                             (MAX(USER_BLOCK_SIZE, PARAMETER_BLOCK_SIZE))

//wake type
/*****************************************************/
#define WAKE_TYPE_NULL                              0x0000
#define WAKE_TYPE_FINGER                            0x0001
#define WAKE_TYPE_RESET_KEY                         0x0002
#define WAKE_TYPE_MATRIX_KEY                        0x0008
#define WAKE_TYPE_HALL                              0x0010
#define WAKE_TYPE_AUTO_LOCK                         0x0020
#define WAKE_TYPE_NFC                               0x0040
#define WAKE_TYPE_BLUE                              0x0080

/*******************  wake source  *******************/
#define WAKE_SOURCE_NULL                            (0x0000)
#define WAKE_SOURCE_KEY_BOARD                       (0x0001)
#define WAKE_SOURCE_FINGER                          (0x0002)
#define WAKE_SOURCE_NFC                             (0x0004)
#define WAKE_SOURCE_TAMPER_KEY                      (0x0008)
#define WAKE_SOURCE_RADAR                           (0x0040)
#define WAKE_SOURCE_LOCK_UART                       (0x0080)

/*****************************************************/


//combine key board
#define COMBINE_KEY_BOARD_AGING_TEST                (555)           //555+#
#define COMBINE_KEY_BOARD_DEVICE_TEST               (258)           //258+#
#define COMBINE_KEY_BOARD_VERSION                   (114)           //114+#
#define COMBINE_KEY_BOARD_TIME                      (299)           //299+#
#define COMBINE_KEY_BOARD_FINGER_WAKE               (101)           
#define COMBINE_KEY_BOARD_PASSWORD_WAKE             (102)           
#define COMBINE_KEY_BOARD_NFC_WAKE                  (103)           
#define COMBINE_KEY_BOARD_STATE_WAKE                (104)       
#define COMBINE_KEY_BOARD_BT_WAKE                   (105)
#define COMBINE_KEY_BOARD_AUTOLOCK_WAKE             (106)
#define COMBINE_KEY_BOARD_RESET_WAKE                (107)
#define COMBINE_KEY_BOARD_ALL_WAKE                  (108)      
#define COMBINE_KEY_BOARD_WAKE_CLR                  (109)       
#define COMBINE_KEY_BOARD_WAKE_UP_CNT               (110)

//warn time
#define TAMPER_WARN_KEEP_TIME                       (60 * 1000)      //unit: ms
#define TAMPER_WARN_PERIOD_TIME                     (3 * 1000)      //unit: ms

#define VACATION_WARN_KEEP_TIME                     (60 *1000)      //unit: ms
#define VACATION_WARN_PERIOD_TIME                   (3 *1000)      //unit: ms
#define VACATION_WARN_FLASH_PERIOD_TIME             (300)      //unit: ms
#define MOTOR_WARN_KEEP_TIME                        (30 *1000)      //unit: ms

//vacation warn lock flag
#define VACATION_WARN_FLAG_MIN                      Disabled
#define VACATION_WARN_FLAG_MAX                      Enabled
#define VACATION_WARN_FLAG_DEFAULT                  Disabled

//locked rotor flag
#define LOCKED_ROTOR_WARM_FLAG_MIN                  Disabled
#define LOCKED_ROTOR_WARM_FLAG_MAX                  Enabled
#define LOCKED_ROTOR_WARM_FLAG_DEFAULT              Disabled

//ble activation
#define BLE_ACTIVATION_FLAG_MIN                     Disabled        //未激活
#define BLE_ACTIVATION_FLAG_MAX                     Enabled         //已激活
#define BLE_ACTIVATION_FLAG_DEFAULT                 Disabled        //默认未激活

//ble net status
#define BLE_NET_STATUS_FLAG_MIN                     Disabled        //未启用
#define BLE_NET_STATUS_FLAG_MAX                     Enabled         //已启用
#define BLE_NET_STATUS_FLAG_DEFAULT                 Disabled        //默认未启用

//ble reset status
#define BLE_RESET_STATUS_MIN                        Disabled        //非重置
#define BLE_RESET_STATUS_MAX                        Enabled         //已重置
#define BLE_RESET_STATUS_DEFAULT                    Disabled        //默认非重置

//ble bing flag
#define BLE_BING_FLAG_MIN                           Disabled        //蓝牙未绑定
#define BLE_BING_FLAG_MAX                           Enabled         //蓝牙已绑定
#define BLE_BING_FLAG_DEFAULT                       Disabled        //默认未绑定

//ble time zone
#define BLE_TIME_ZONE_MIN                           0               //UTC-12:00 西12时区
#define BLE_TIME_ZONE_MAX                           104             //UTC+14:00 东12时区+2个特殊时区
#define BLE_TIME_ZONE_DEFAULT                       48              //UTC 0：00 UTC时间

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
