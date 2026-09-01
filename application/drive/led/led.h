/**
 * Copyright(C) 2024 GZ-OB. All Rights Reserved
 * File: led.h
 * Desc: 
 * Ver : V1.00
 * Revision: Ren
 * Date: 2024-12-02
 *  */

#ifndef LED__HH
#define LED__HH

#include "config.h"
#include "hal_gpio.h"

/*****************Macro****************/
#define SET_RED_LED_OPEN()                      HAL_GPIO_Write(RED_LED_GPIO,RED_LED_PIN,1)
#define SET_RED_LED_CLOSE()                     HAL_GPIO_Write(RED_LED_GPIO,RED_LED_PIN,0)

#define SET_GREEN_LED_OPEN()                    HAL_GPIO_Write(GREEN_LED_GPIO,GREEN_LED_PIN,1)
#define SET_GREEN_LED_CLOSE()                   HAL_GPIO_Write(GREEN_LED_GPIO,GREEN_LED_PIN,0)

#define SET_BLUE_LED_OPEN()                     HAL_GPIO_Write(BLUE_LED_GPIO,BLUE_LED_PIN,1)
#define SET_BLUE_LED_CLOSE()                    HAL_GPIO_Write(BLUE_LED_GPIO,BLUE_LED_PIN,0)

#define SET_WHITE_LED_OPEN()                    HAL_GPIO_Write(WHITE_LED_GPIO,WHITE_LED_PIN,1)
#define SET_WHITE_LED_CLOSE()                   HAL_GPIO_Write(WHITE_LED_GPIO,WHITE_LED_PIN,0)

#define SET_ALL_LED_CLOSE()                     {SET_RED_LED_CLOSE(); SET_GREEN_LED_CLOSE(); SET_BLUE_LED_CLOSE();SET_WHITE_LED_CLOSE();}

#define RED_LED_INIT()                                                                        \
    {                                                                                         \
        HAL_GPIO_Init(RED_LED_GPIO, RED_LED_PIN, HAL_GPIO_MODE_OUTPUT_PP, HAL_GPIO_PULL_LOW); \
        SET_RED_LED_CLOSE();                                                                  \
    }
#define GREEN_LED_INIT()                                                                          \
    {                                                                                             \
        HAL_GPIO_Init(GREEN_LED_GPIO, GREEN_LED_PIN, HAL_GPIO_MODE_OUTPUT_PP, HAL_GPIO_PULL_LOW); \
        SET_GREEN_LED_CLOSE();                                                                    \
    }
#define BLUE_LED_INIT()                                                                         \
    {                                                                                           \
        HAL_GPIO_Init(BLUE_LED_GPIO, BLUE_LED_PIN, HAL_GPIO_MODE_OUTPUT_PP, HAL_GPIO_PULL_LOW); \
        SET_BLUE_LED_CLOSE();                                                                   \
    }
#define WHITE_LED_INIT()                                                                          \
    {                                                                                             \
        HAL_GPIO_Init(WHITE_LED_GPIO, WHITE_LED_PIN, HAL_GPIO_MODE_OUTPUT_PP, HAL_GPIO_PULL_LOW); \
        SET_WHITE_LED_CLOSE();                                                                    \
    }

#define LOGO_RED_LED_OPEN()    \
    {                          \
        SET_RED_LED_OPEN();    \
        SET_GREEN_LED_CLOSE(); \
        SET_BLUE_LED_CLOSE();  \
    }
#define LOGO_GREEN_LED_OPEN() \
    {                         \
        SET_RED_LED_CLOSE();  \
        SET_GREEN_LED_OPEN(); \
        SET_BLUE_LED_CLOSE(); \
    }
#define LOGO_BLUE_LED_OPEN()   \
    {                          \
        SET_RED_LED_CLOSE();   \
        SET_GREEN_LED_CLOSE(); \
        SET_BLUE_LED_OPEN();   \
    }
#define LOGO_LED_CLOSE()       \
    {                          \
        SET_RED_LED_CLOSE();   \
        SET_GREEN_LED_CLOSE(); \
        SET_BLUE_LED_CLOSE();  \
    }

/*****************Enum*****************/
typedef enum{
    LOGO_LED_COLOR_IDLE = 0,
    LOGO_LED_COLOR_GREEN,
    LOGO_LED_COLOR_RED,
    LOGO_LED_COLOR_BLUE,
}logo_led_color_t;

/****************Struct****************/


/***************Variable***************/


/***************Function***************/
void ledInit(void);
void ledPowerOnInit(void);
void keyBoardLedDrive(uint8_t ledSw);
void logoLedDrive(uint8_t color);
/**************************************/

#endif 
