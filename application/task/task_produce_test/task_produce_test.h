/**
 * Copyright(C) 2024 GZ-OB. All Rights Reserved
 * File: task_produce_test.h
 * Desc: 
 * Ver : V1.00
 * Revision: Ren
 * Date: 2025-01-10
 *  */

#ifndef TASK_PRODUCE_TEST__HH
#define TASK_PRODUCE_TEST__HH

#include "config.h"
#include "bsp_produce_test.h"


typedef bsp_produce_test_callback_t task_produce_test_callback_t;
/*****************Macro****************/


/*****************Enum*****************/
typedef enum{
    PRODUCE_TEST_IDLE = 0,      //非产测状态
    PRODUCE_TEST_PCBA,          //PCBA
    PRODUCE_TEST_DEVICE,        //整机测试

}produce_test_state_t;

/****************Struct****************/
typedef struct{
    uint8_t type;
    uint8_t step;
    
}produce_test_task_attribute_t;

typedef struct{
    const bsp_produce_test_drive_io_t *io;

    /**
     * @brief Attribute of this driver
     * 
     */
    produce_test_task_attribute_t attribute;

}produce_test_task_driver_t;

/***************Variable***************/


/***************Function***************/
void produceTestTaskInit(void);
void produceTestTaskLoop(void);
uint8_t produceTestTaskGetType(void);
void produceTestTaskUartConfig(uint8_t initFlag);
void produceTestTaskSetType(uint8_t testType);
uint8_t produceTestTaskKeyBoard(uint8_t keyValue);
void produceTestTaskMotor(uint32_t value);
void produceTestTaskNfc(uint8_t flag);
void produceTestTaskKey(uint8_t value);

void produceTestTaskSetStep(uint8_t step);
uint8_t produceTestTaskGetStep(void);

/**************************************/

#endif 
