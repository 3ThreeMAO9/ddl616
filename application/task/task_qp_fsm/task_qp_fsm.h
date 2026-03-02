/**
 * Copyright(C) 2024 GZ-OB. All Rights Reserved
 * File: task_qp_fsm.h
 * Desc: 
 * Ver : V1.00
 * Revision: Ren
 * Date: 2025-01-15
 *  */

#ifndef TASK_QP_FSM__HH
#define TASK_QP_FSM__HH

#include "config.h"
#include "bsp_qp_fsm.h"


/*****************Macro****************/


/*****************Enum*****************/


/****************Struct****************/

typedef struct{
    const bsp_qp_fsm_drive_io_t *io;

    /**
     * @brief Attribute of this driver
     * 
     */

}qp_fsm_task_driver_t;

/***************Variable***************/


/***************Function***************/
void qp_fsm_task_init(void);
void qp_fsm_task_loop(void);

/**************************************/

#endif 
