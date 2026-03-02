/**
 * Copyright (c) 2025 GZ-OB, All rights reserved.
 * File name: tamper_key.h
 * Desc:
 * Version: 1.0.0
 * Revision: James_Zhang
 * Date: 2025-12-22
 */

#ifndef __TAMPER_KEY_H__
#define __TAMPER_KEY_H__

#include "config.h"
#include "hal_gpio.h"

/***********Macro***********/
#define SET_TAMPER_KEY_LEVEL(_level)    (HAL_GPIO_Write(TAMPER_KEY_GPIO, TAMPER_KEY_PIN, _level))
#define READ_TAMPER_KEY_LEVEL()         (HAL_GPIO_Read(TAMPER_KEY_GPIO,TAMPER_KEY_PIN))

#define READ_TAMPER_KEY_STATE()         (HAL_GPIO_ReadIntState(TAMPER_KEY_GPIO,TAMPER_KEY_PIN))   //读取GPIO中断信号
#define CLEAR_TAMPER_KEY_STATE()        (HAL_GPIO_ClearIntState(TAMPER_KEY_GPIO,TAMPER_KEY_PIN))  //清空GPIO中断信号

#define TAMPER_KEY_INIT(_level)                                                                  \
    do                                                                                           \
    {                                                                                            \
        SET_TAMPER_KEY_LEVEL(_level);                                                            \
        HAL_GPIO_Init(TAMPER_KEY_GPIO, TAMPER_KEY_PIN, HAL_GPIO_MODE_INPUT, HAL_GPIO_PULL_NONE); \
    } while (0)

#define TAMPER_KEY_IRQ_FALLING()                                                   \
    do                                                                             \
    {                                                                              \
        HAL_GPIO_EnableIRQ(TAMPER_KEY_GPIO, TAMPER_KEY_PIN, HAL_GPIO_IRQ_FALLING); \
        CLEAR_TAMPER_KEY_STATE();                                                  \
    } while (0)

#define TAMPER_KEY_IRQ_RISING()                                                   \
    do                                                                            \
    {                                                                             \
        HAL_GPIO_EnableIRQ(TAMPER_KEY_GPIO, TAMPER_KEY_PIN, HAL_GPIO_IRQ_RISING); \
        CLEAR_TAMPER_KEY_STATE();                                                 \
    } while (0)

/***********Enum***********/

/***********Struct***********/

/***********Variable***********/

/***********Function***********/
void tamper_key_init(void);
void tamper_key_sleep(uint8_t event);
uint8_t read_tamper_key_level(uint8_t id);
uint16_t tamper_key_check_wake(void);

/*****************************/

#endif /* __TAMPER_KEY_H__ */
