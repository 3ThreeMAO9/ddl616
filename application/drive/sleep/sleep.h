/**
 * Copyright(C) 2024 GZ-OB. All Rights Reserved
 * File: sleep.h
 * Desc: 
 * Ver : V1.00
 * Revision: Ren
 * Date: 2024-12-03
 *  */

#ifndef SLEEP__HH
#define SLEEP__HH

#include "config.h"

typedef uint8_t (*sleep_event_callback_t)(uint8_t);

/*****************Macro****************/
#define SBR_ADDR                            0xE000ED10
#define IDLE_GPIO_SLEEP_MODE                GPIO_PINMODE_PUSH_PULL

/*****************Enum*****************/


/****************Struct****************/


/***************Variable***************/


/***************Function***************/
void sleep_loop(void);
void unused_gpio_init(void);
void sleep_event_register_callback(sleep_event_callback_t callback);

/**************************************/

#endif 
