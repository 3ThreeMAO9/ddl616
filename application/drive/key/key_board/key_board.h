/**
 * Copyright (c) 2025 GZ-OB, All rights reserved.
 * File name: key_board.h
 * Desc:
 * Version: 1.0.0
 * Revision: James_Zhang
 * Date: 2025-12-01
 */

#ifndef KEY_BOARD_HH
#define KEY_BOARD_HH

#include "config.h"

/************************Macro************************/

/************************Enum************************/

/************************Struct************************/
typedef struct {
    uint8_t (*init)(uint8_t*);        // 初始化函数指针
    uint8_t (*scan)(uint8_t*);        // 扫描函数指针
    uint8_t (*is_wake)(void);         // 唤醒检测函数指针
    uint8_t (*sleep)(void);           // 休眠函数指针
    uint8_t (*wake)(uint8_t);         // 唤醒配置函数指针
    uint8_t (*parameter)(uint8_t*);   // 参数配置函数指针
} keyboard_handle_t;

/************************Variable************************/

/************************Function************************/
uint8_t keyboard_init(void* parameter);
uint8_t keyboard_scan(void);
uint8_t keyboard_sleep_config(void);
uint8_t keyboard_wake_config(uint8_t keyboard_wake);
uint8_t keyboard_set_parameter(void* packet);
uint8_t is_keyboard_wake(void);
uint8_t read_keyboard_level(uint8_t id);
uint8_t read_keyboard_value(void);

#endif

