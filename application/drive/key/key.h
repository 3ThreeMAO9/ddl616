/******************************************************************
 * Copyright(c) 2024 GZ-OB. All Rights Reserved
 * File: key.h
 * Desc: 
 * Ver : V1.00
 * Revision: Ren
 * Date: 2025-1-17
 ******************************************************************/
#ifndef MODULE_KEY_HH
#define MODULE_KEY_HH

#include "config.h"
#include "multi_button.h"

typedef void (*key_event_callback_t)(uint8_t, uint8_t);

/*************************macro*************************/
#define KEY_SCAN_TIME_OUT                (TICKS_INTERVAL)  //unit: ms
#define KEY_CNT                          (13)

#define KEY_BOARD_LONG_PRESS_TIME        (1000 / KEY_SCAN_TIME_OUT)

#define TAMPER_KEY_LONG_PRESS_TIME       (KEY_SCAN_TIME_OUT / KEY_SCAN_TIME_OUT)

#define KEY_BAORD_SCAN_TIME              (10)

/*************************Enum*************************/
typedef enum{
    KEY_EVENT_RESET_PRESS = 0,          //长按复位未松手
    KEY_EVENT_RESET_PRESS_RELEASE,      //长按复位松手
    KEY_EVENT_RESET_SHORT_PRESS_RELEASE,//短按复位松手
    KEY_EVENT_RESET_PRESS_TEST,
}key_event_t;

typedef enum{
    KEY_EVENT_TAMPER_PRESS = 0,          // 长按防撬未松手
    KEY_EVENT_TAMPER_PRESS_RELEASE,      // 长按防撬松手
    KEY_EVENT_TAMPER_SHORT_PRESS_RELEASE,// 短按防撬松手
    KEY_EVENT_TAMPER_PRESS_DOWN,         // 防撬按下
    KEY_EVENT_TAMPER_PRESS_UP,           // 防撬松手
}key_tamper_event_t;

typedef enum{
    KEY_EVENT_CLOSE_PRESS = 0,          //长按关锁未松手
    KEY_EVENT_CLOSE_PRESS_RELEASE,      //长按关锁松手
    KEY_EVENT_CLOSE_SHORT_PRESS_RELEASE,//短按关锁松手
    KEY_EVENT_CLOSE_PRESS_TEST,
}key_close_event_t;

typedef enum{
    KEY_TYPE_KEY_BOARD = 0,
    KEY_TYPE_TAMPER_KEY,
    KEY_TYPE_LONG_RELEASE,
    KEY_TYPE_CLOSE_KEY,
    KEY_TYPE_CNT,
}key_type_t;

/*************************Struct*************************/
typedef struct{
    uint8_t enable;
    uint8_t busy;
    uint16_t pressCnt;
    uint32_t timeOut;
}tamper_key_handle_t;

typedef struct{
    uint8_t enable;
    uint8_t busy;
    uint8_t value;
    uint16_t pressCnt;
    uint32_t timeOut;
}key_board_handle_t;

typedef struct{
    tamper_key_handle_t tamper_key;
    key_board_handle_t keyBoard;
    struct Button keyBtn[KEY_CNT + 2];
    uint32_t timeOut;
}key_handle_t;

/*************************Function*************************/
void module_keyScanLoop(void);
void keyEventRegister_callback(key_event_callback_t callback);
uint8_t module_keyWake(void);
uint8_t module_keySleep(uint8_t type);
void module_keyWakeConfig(uint8_t keyboard_wake);
uint8_t module_isBusyKey(uint8_t type);
uint8_t module_keyInit(uint8_t type);
void module_keySwitch(uint8_t type, uint8_t flag);
void module_key_pause(uint32_t value);
void module_keyTestMode(uint8_t mode);
void module_keyParameter(void* packet);

/**********************************************************/

#endif /* MODULE_KEY_HH */

