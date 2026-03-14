/**
 * Copyright(C) 2024 GZ-OB. All Rights Reserved
 * File: task_sleep.h
 * Desc: 
 * Ver : V1.00
 * Revision: Ren
 * Date: 2025-01-15
 *  */

#ifndef TASK_SLEEP__HH
#define TASK_SLEEP__HH

#include "config.h"
#include "bsp_sleep.h"


/*****************Macro****************/


/*****************Enum*****************/


/****************Struct****************/
#if (Enabled == WAKE_STAT_ENABLE)

extern u8 Wake_allCnt;
extern u8 Wake_keyBoardCnt;
extern u8 Wake_setKeyCnt;
extern u8 Wake_switchKeyCnt;
extern u8 Wake_NFCCnt;
extern u8 Wake_doorStateCnt;
extern u8 Wake_hotWarnCnt;
extern u8 Wake_remoteCnt;

#endif
typedef enum{
    WAKE_UP_TYPE_NULL,
    WAKE_UP_TYPE_KEY_BOARD,
    WAKE_UP_TYPE_FINGER,
    WAKE_UP_TYPE_NFC,
    WAKE_UP_TYPE_TAMPER_KEY,
    WAKE_UP_TYPE_RADAR,
    WAKE_UP_TYPE_CNT
}wake_up_type_e;



typedef struct{
    uint8_t flag;
    uint16_t wake_source;
    
}sleep_task_attribute_t;

typedef struct{
    const bsp_sleep_drive_io_t *io;

    /**
     * @brief Attribute of this driver
     * 
     */
    sleep_task_attribute_t attribute;
    
}sleep_task_driver_t;

/***************Variable***************/


/***************Function***************/
void sleep_task_init(void);
void sleep_task_loop(void);
void sleep_task_set_flag(void);
void reset7258_handle(void);

/**************************************/

#endif 
