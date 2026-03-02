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

#define READ_TOUCH_I2C_SDA() (HAL_GPIO_Read(TOUCH_I2C_SDA_GPIO, TOUCH_I2C_SDA_PIN))
#define READ_TOUCH_WAKE() (HAL_GPIO_Read(TOUCH_WAKE_GPIO, TOUCH_WAKE_PIN))

#define SET_TOUCH_RESET(_level) (HAL_GPIO_Write(TOUCH_RESET_GPIO, TOUCH_RESET_PIN, _level))

#define TOUCH_I2C_SCL_INIT(_level)                                                                            \
    do                                                                                                        \
    {                                                                                                         \
        HAL_GPIO_Write(TOUCH_I2C_SCL_GPIO, TOUCH_I2C_SCL_PIN, _level);                                        \
        HAL_GPIO_Init(TOUCH_I2C_SCL_GPIO, TOUCH_I2C_SCL_PIN, HAL_GPIO_MODE_INPUT_PULLUP, HAL_GPIO_PULL_NONE); \
    } while (0)

#define TOUCH_I2C_SDA_INIT(_level)                                                                            \
    do                                                                                                        \
    {                                                                                                         \
        HAL_GPIO_Write(TOUCH_I2C_SDA_GPIO, TOUCH_I2C_SDA_PIN, _level);                                        \
        HAL_GPIO_Init(TOUCH_I2C_SDA_GPIO, TOUCH_I2C_SDA_PIN, HAL_GPIO_MODE_INPUT_PULLUP, HAL_GPIO_PULL_NONE); \
    } while (0)

#define TOUCH_WAKE_INIT(_level)                                                                  \
    do                                                                                           \
    {                                                                                            \
        HAL_GPIO_Write(TOUCH_WAKE_GPIO, TOUCH_WAKE_PIN, _level);                                 \
        HAL_GPIO_Init(TOUCH_WAKE_GPIO, TOUCH_WAKE_PIN, HAL_GPIO_MODE_INPUT, HAL_GPIO_PULL_NONE); \
    } while (0)

#define TOUCH_RESET_INIT(_level)                                                                       \
    do                                                                                                 \
    {                                                                                                  \
        HAL_GPIO_Write(TOUCH_RESET_GPIO, TOUCH_RESET_PIN, _level);                                     \
        HAL_GPIO_Init(TOUCH_RESET_GPIO, TOUCH_RESET_PIN, HAL_GPIO_MODE_OUTPUT_PP, HAL_GPIO_PULL_NONE); \
    } while (0)

#define TOUCH_WAKE_PIN_ENABLE() (HAL_GPIO_EnableIRQ(TOUCH_WAKE_GPIO, TOUCH_WAKE_PIN, HAL_GPIO_IRQ_FALLING))
#define TOUCH_WAKE_PIN_DISABLE() (HAL_GPIO_DisableIRQ(TOUCH_WAKE_GPIO, TOUCH_WAKE_PIN))

#define TOUCH_WAKE_READ_INTSTATE() (HAL_GPIO_ReadIntState(TOUCH_WAKE_GPIO, TOUCH_WAKE_PIN))
#define TOUCH_WAKE_CLEAR_INTSTATE() (HAL_GPIO_ClearIntState(TOUCH_WAKE_GPIO, TOUCH_WAKE_PIN))

/***********Enum***********/

/***********Struct***********/

/***********Variable***********/

/***********Function***********/

/*****************************/

#endif /* __GTX314L_CONFIG_H__ */
