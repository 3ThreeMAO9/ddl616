/**
 * Copyright (c) 2025 GZ-OB, All rights reserved.
 * File name: gtx314l_config.h
 * Desc:
 * Version: 1.0.0
 * Revision: James_Zhang
 * Date: 2025-12-01
 */

#ifndef __GTX314L_CONFIG_H__
#define __GTX314L_CONFIG_H__

#include "config.h"
#include "hal_gpio.h"

/***********Macro***********/
#define GTX314L_IIC_ADDR (0xB6 >> 1)

#define SET_TOUCH_WAKE_PIN_OPEN()       (TOUCH_WAKE_GPIO->DATA |= TOUCH_WAKE_PIN)
#define SET_TOUCH_WAKE_PIN_CLOSE()      (TOUCH_WAKE_GPIO->DATA &= ~TOUCH_WAKE_PIN)

#define SET_TOUCH_RESET_PIN_OPEN()      (TOUCH_RESET_GPIO->DATA |= TOUCH_RESET_PIN)
#define SET_TOUCH_RESET_PIN_CLOSE()     (TOUCH_RESET_GPIO->DATA &= ~TOUCH_RESET_PIN)

#define SET_TOUCH_SDA_PIN_OPEN()        (TOUCH_I2C_SDA_GPIO->DATA |= TOUCH_I2C_SDA_PIN)
#define SET_TOUCH_SDA_PIN_CLOSE()       (TOUCH_I2C_SDA_GPIO->DATA &= ~TOUCH_I2C_SDA_PIN)

#define SET_TOUCH_SCL_PIN_OPEN()        (TOUCH_I2C_SCL_GPIO->DATA |= TOUCH_I2C_SCL_PIN)
#define SET_TOUCH_SCL_PIN_CLOSE()       (TOUCH_I2C_SCL_GPIO->DATA &= ~TOUCH_I2C_SCL_PIN)

#define READ_TOUCH_I2C_SDA()            (TOUCH_I2C_SDA_GPIO->PIN & TOUCH_I2C_SDA_PIN)
#define READ_TOUCH_WAKE()               (TOUCH_WAKE_GPIO->PIN & TOUCH_WAKE_PIN)

#define TOUCH_I2C_SCL_INIT()                                                                               \
    do                                                                                                     \
    {                                                                                                      \
        SET_TOUCH_SCL_PIN_OPEN();                                                                          \
        GPIO_SetPinMFType(TOUCH_I2C_SCL_GPIO, TOUCH_I2C_SCL_PIN, GPIO_MF_TYPE_GPIO, GPIO_PINMODE_PULL_UP); \
    } while (0)

#define TOUCH_I2C_SDA_INIT()                                                                               \
    do                                                                                                     \
    {                                                                                                      \
        SET_TOUCH_SDA_PIN_OPEN();                                                                          \
        GPIO_SetPinMFType(TOUCH_I2C_SDA_GPIO, TOUCH_I2C_SDA_PIN, GPIO_MF_TYPE_GPIO, GPIO_PINMODE_PULL_UP); \
    } while (0)

#define TOUCH_WAKE_INIT()                                                                          \
    do                                                                                             \
    {                                                                                              \
        SET_TOUCH_WAKE_PIN_OPEN();                                                                 \
        GPIO_SetPinMFType(TOUCH_WAKE_GPIO, TOUCH_WAKE_PIN, GPIO_MF_TYPE_GPIO, GPIO_PINMODE_INPUT); \
    } while (0)

#define TOUCH_RESET_INIT()                                                                               \
    do                                                                                                   \
    {                                                                                                    \
        SET_TOUCH_RESET_PIN_CLOSE();                                                                     \
        GPIO_SetPinMFType(TOUCH_RESET_GPIO, TOUCH_RESET_PIN, GPIO_MF_TYPE_GPIO, GPIO_PINMODE_PUSH_PULL); \
    } while (0)

#define TOUCH_WAKE_PIN_ENABLE()         {GPIO_EnableINT(TOUCH_WAKE_GPIO, TOUCH_WAKE_PIN, GPIO_INTMODE_FALLING_EDGE);}
#define TOUCH_WAKE_PIN_DISABLE()        {GPIO_EnableINT(TOUCH_WAKE_GPIO, TOUCH_WAKE_PIN, GPIO_INTMODE_DISABLE);}

#define TOUCH_WAKE_READ_INTSTATE()      (hal_read_gpio_int_flag(TOUCH_WAKE_GPIO, TOUCH_WAKE_PIN))
#define TOUCH_WAKE_CLEAR_INTSTATE()     (hal_clear_gpio_int_flag(TOUCH_WAKE_GPIO, TOUCH_WAKE_PIN))

/***********Enum***********/

/***********Struct***********/

/***********Variable***********/

/***********Function***********/

/*****************************/

#endif /* __GTX314L_CONFIG_H__ */
