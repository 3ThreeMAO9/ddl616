/**
 * Copyright(C) 2024 GZ-OB. All Rights Reserved
 * File: FM17622_CFG.h
 * Desc: 
 * Ver : V1.00
 * Revision: Ren
 * Date: 2025-11-14
 *  */

#ifndef FM17622_CFG__HH
#define FM17622_CFG__HH

#include "config.h"
#include "FM17622_REG.h"
#include "DEVICE_CFG.h"


/*****************Macro****************/
#define BIT0               0x01
#define BIT1               0x02
#define BIT2               0x04
#define BIT3               0x08
#define BIT4               0x10
#define BIT5               0x20
#define BIT6               0x40
#define BIT7               0x80


/*****************Enum*****************/


/****************Struct****************/


/***************Variable***************/


/***************Function***************/
void FM17622_HardReset(void);
void FM17622_DeepSleep(void);
uint8_t FM17622_Version(void);

unsigned char FM17622_SoftReset(void);
uint8_t SetCW(uint8_t cw_mode);

uint8_t SetReg_Ext(uint8_t ext_reg_address,uint8_t ext_reg_data);
uint8_t GetReg_Ext(uint8_t ext_reg_address,uint8_t* ext_reg_data);
void ModifyReg_Ext(uint8_t ExtRegAddr, uint8_t mask, uint8_t set);

void Clear_FIFO(void);
void Write_FIFO(uint8_t *fifo_data, uint8_t length);
void Read_FIFO(uint8_t *fifo_data, uint8_t length);

uint8_t FM17622_Transceive7Bits(uint8_t data_7bit, uint8_t* pOutData, uint8_t* pOutLenbit);

/**************************************/

#endif 

