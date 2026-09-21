/**
 * Copyright(C) 2024 GZ-OB. All Rights Reserved
 * File: module_produce_test.h
 * Desc: PCBA浜ф祴
 * Ver : V1.00
 * Revision: Ren
 * Date: 2025-01-08
 *  */

#ifndef MODULE_PCBA_TEST__HH
#define MODULE_PCBA_TEST__HH

#include "config.h"

typedef void (*produce_test_callback_t)(uint8_t, uint8_t);

/*****************Macro****************/
#define PRODUCE_PCBA_TEST_TIME_OUT_MAX                  (60 * 1000)             //unit: ms
#define PRODUCE_DEVICE_TEST_TIME_OUT_MAX                (20 * 1000)             //unit: ms
#define AGING_TEST_TIME_OUT_2H                          (2 * 60 * 60 * 1000)    //unit: ms
#define AGING_TEST_TIME_OUT_4H                          (4 * 60 * 60 * 1000)    //unit: ms
#define AGING_TEST_TIME_OUT_8H                          (8 * 60 * 60 * 1000)    //unit: ms
#define AGING_TEST_PERIOD_TIME_OUT                      (10 * 1000)             //unit: ms

/*****************Enum*****************/
typedef enum{
    PCBA_TEST_STEP_IDLE = 0,
    PCBA_TEST_STEP_VERSION,
    PCBA_TEST_STEP_IO,
    PCBA_TEST_STEP_MOTOR,
    PCBA_TEST_STEP_KEY_BOARD,
    PCBA_TEST_STEP_WRITE_SN,
    PCBA_TEST_STEP_READ_SN,
    PCBA_TEST_STEP_SLEEP,
    PCBA_TEST_STEP_KEY,
}pcba_test_step_t;

typedef enum{
    DEVICE_TEST_STEP_IDLE = 0,
    DEVICE_TEST_STEP_KEY_BOARD,
    DEVICE_TEST_STEP_VERIFY_FINGER,
    DEVICE_TEST_STEP_REGISTER_FINGERPRINT,
    DEVICE_TEST_STEP_DELETE_FINGERPRINT,
    DEVICE_TEST_STEP_VERIFY_NFC,
    DEVICE_TEST_STEP_MOTOR_RESET,
    DEVICE_TEST_STEP_UNLOCK,
    DEVICE_TEST_STEP_LOCK,
    DEVICE_TEST_STEP_SLEEP,
    DEVICE_TEST_STEP_RESET,
    DEVICE_TEST_STEP_LOCK_KEY,
}device_test_step_t;

typedef enum{
    PCBA_MOTOR_TEST_STOP = 0,
    PCBA_MOTOR_TEST_FORWARD,
    PCBA_MOTOR_TEST_REVERSE,
    
    PCBA_MOTOR_TEST_CNT,

}pcba_motor_test_handle_t;

typedef enum{
    PCBA_KEY_TEST_START = 0,
    PCBA_KEY_TEST_STOP,
    
    PCBA_KEY_TEST_CNT,
}pcba_key_test_handle_t;

typedef enum{
    PCBA_KEY_BOARD_TEST_PRESS_REPORT_DISABLE = 0,
    PCBA_KEY_BOARD_TEST_PRESS_REPORT_ENABLE,

}pcba_key_board_test_report_t;

/****************Struct****************/
typedef struct{
    uint8_t step;
    uint8_t pressReport;
    uint8_t tab[KEY_CNT];

}pcba_key_board_test_t;

typedef struct{
    uint8_t flag;
    uint8_t step;

    pcba_key_board_test_t keyBoard;

}produce_test_handle_t;

/***************Variable***************/


/***************Function***************/
void produceTestInit(void);
void produceTestLoop(void);
void produceTestSleep(void);
void produceTestHwConfig(uint8_t initFlag);
void produceTestEventRegister_callback(produce_test_callback_t callback);
void pcbaNfcTestPressHandle(uint8_t flag);
uint8_t pcbaTestGetHandleStep(void);

uint8_t pcbaKeyBoardTestPressHandle(uint8_t keyValue);
void pcbaMotorTestHandle(uint32_t value);
void pcbaKeyTestHandle(uint8_t value);
void exit_produce_test(void);
uint8_t isTestEnable(void);
uint8_t isTesting(void);
uint8_t isLEDHoldOn(void);
/**************************************/

#endif 
