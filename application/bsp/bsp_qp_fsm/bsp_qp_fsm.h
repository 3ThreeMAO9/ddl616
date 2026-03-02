/**
 * Copyright(C) 2024 GZ-OB. All Rights Reserved
 * File: bsp_qp_fsm.h
 * Desc: 
 * Ver : V1.00
 * Revision: Ren
 * Date: 2025-01-15
 *  */

#ifndef BSP_QP_FSM__HH
#define BSP_QP_FSM__HH

#include "config.h"

/*****************Macro****************/


/*****************Enum*****************/


/****************Struct****************/
typedef struct{
    void (*init)(void);
    void (*loop)(void);

}bsp_qp_fsm_drive_io_t;

/***************Variable***************/


/***************Function***************/
const bsp_qp_fsm_drive_io_t* bsp_qp_fsm_get_driver(void);

/**************************************/

#endif 
