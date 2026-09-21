/**
 * Copyright(C) 2024 GZ-OB. All Rights Reserved
 * File: bsp_produce_test.h
 * Desc: 
 * Ver : V1.00
 * Revision: Ren
 * Date: 2025-01-10
 *  */

#ifndef BSP_PRODUCE_TEST__HH
#define BSP_PRODUCE_TEST__HH

#include "config.h"
#include "module_produce_test.h"

typedef produce_test_callback_t bsp_produce_test_callback_t;

/*****************Macro****************/


/*****************Enum*****************/


/****************Struct****************/
typedef struct{
    void (*init)(void);
    // void (*uartConfig)(uint8_t);
    void (*loop)(void);
    // uint8_t (*keyBoard)(uint8_t);
    // void (*motor)(uint32_t);
    // void (*nfc)(uint8_t);
    // void (*key)(uint8_t);
}bsp_produce_test_drive_io_t;

/***************Variable***************/


/***************Function***************/
const bsp_produce_test_drive_io_t* bsp_produce_test_get_driver(bsp_produce_test_callback_t callback);

/**************************************/

#endif 
