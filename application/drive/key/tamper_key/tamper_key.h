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
#define SET_TAMPER_KEY_OPEN()           (TAMPER_KEY_GPIO->DATA |= TAMPER_KEY_PIN)
#define SET_TAMPER_KEY_CLOSE()          (TAMPER_KEY_GPIO->DATA &= ~TAMPER_KEY_PIN)
#define READ_TAMPER_KEY_LEVEL()         (TAMPER_KEY_GPIO->PIN & TAMPER_KEY_PIN)

#define READ_TAMPER_KEY_STATE()         (hal_read_gpio_int_flag(TAMPER_KEY_GPIO,TAMPER_KEY_PIN))   //读取GPIO中断信号
#define CLEAR_TAMPER_KEY_STATE()        (hal_clear_gpio_int_flag(TAMPER_KEY_GPIO,TAMPER_KEY_PIN))  //清空GPIO中断信号

#define TAMPER_KEY_INIT()                                                                          \
    do                                                                                             \
    {                                                                                              \
        SET_TAMPER_KEY_OPEN();                                                                     \
        GPIO_SetPinMFType(TAMPER_KEY_GPIO, TAMPER_KEY_PIN, GPIO_MF_TYPE_GPIO, GPIO_PINMODE_INPUT); \
    } while (0)

#define TAMPER_KEY_IRQ_FALLING()                                                    \
    do                                                                              \
    {                                                                               \
        GPIO_EnableINT(TAMPER_KEY_GPIO, TAMPER_KEY_PIN, GPIO_INTMODE_FALLING_EDGE); \
        CLEAR_TAMPER_KEY_STATE();                                                   \
    } while (0)

#define TAMPER_KEY_IRQ_RISING()                                                    \
    do                                                                             \
    {                                                                              \
        GPIO_EnableINT(TAMPER_KEY_GPIO, TAMPER_KEY_PIN, GPIO_INTMODE_RISING_EDGE); \
        CLEAR_TAMPER_KEY_STATE();                                                  \
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
