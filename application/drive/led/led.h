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
#define SET_RED_LED_OPEN()                      (RED_LED_GPIO->DATA |= RED_LED_PIN)
#define SET_RED_LED_CLOSE()                     (RED_LED_GPIO->DATA &= ~RED_LED_PIN)

#define SET_GREEN_LED_OPEN()                    (GREEN_LED_GPIO->DATA |= GREEN_LED_PIN)
#define SET_GREEN_LED_CLOSE()                   (GREEN_LED_GPIO->DATA &= ~GREEN_LED_PIN)

#define SET_BLUE_LED_OPEN()                     (BLUE_LED_GPIO->DATA |= BLUE_LED_PIN)
#define SET_BLUE_LED_CLOSE()                    (BLUE_LED_GPIO->DATA &= ~BLUE_LED_PIN)

#define SET_WHITE_LED_OPEN()                    (WHITE_LED_GPIO->DATA |= WHITE_LED_PIN)
#define SET_WHITE_LED_CLOSE()                   (WHITE_LED_GPIO->DATA &= ~WHITE_LED_PIN)

#define SET_ALL_LED_CLOSE()                     {SET_RED_LED_CLOSE(); SET_GREEN_LED_CLOSE(); SET_BLUE_LED_CLOSE();SET_WHITE_LED_CLOSE();}

#define RED_LED_INIT()                                                                           \
    {                                                                                            \
        GPIO_SetPinMFType(RED_LED_GPIO, RED_LED_PIN, GPIO_MF_TYPE_GPIO, GPIO_PINMODE_PUSH_PULL); \
        SET_RED_LED_CLOSE();                                                                     \
    }
#define GREEN_LED_INIT()                                                                             \
    {                                                                                                \
        GPIO_SetPinMFType(GREEN_LED_GPIO, GREEN_LED_PIN, GPIO_MF_TYPE_GPIO, GPIO_PINMODE_PUSH_PULL); \
        SET_GREEN_LED_CLOSE();                                                                       \
    }
#define BLUE_LED_INIT()                                                                            \
    {                                                                                              \
        GPIO_SetPinMFType(BLUE_LED_GPIO, BLUE_LED_PIN, GPIO_MF_TYPE_GPIO, GPIO_PINMODE_PUSH_PULL); \
        SET_BLUE_LED_CLOSE();                                                                      \
    }
#define WHITE_LED_INIT()                                                                             \
    {                                                                                                \
        GPIO_SetPinMFType(WHITE_LED_GPIO, WHITE_LED_PIN, GPIO_MF_TYPE_GPIO, GPIO_PINMODE_PUSH_PULL); \
        SET_WHITE_LED_CLOSE();                                                                       \
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
