/**
 * Copyright(C) 2024 GZ-OB. All Rights Reserved
 * File: DEVICE_CFG.h
 * Desc: 
 * Ver : V1.00
 * Revision: Ren
 * Date: 2025-11-14
 *  */

#ifndef DEVICE_CFG__HH
#define DEVICE_CFG__HH

#include "config.h"
#include "hal_gpio.h"
#include "hal_spi.h"
#include "spi_software.h"

/*****************Macro****************/
#define NFC_SOPT_SPI_SEL                1


#define READ_NFC_IRQ_PIN()              HAL_GPIO_Read(NFC_IRQ_GPIO,NFC_IRQ_PIN)

#define READ_NFC_IRQ_STATE()            HAL_GPIO_ReadIntState(NFC_IRQ_GPIO,NFC_IRQ_PIN)   //读取GPIO中断信号
#define CLEAR_NFC_IRQ_STATE()           HAL_GPIO_ClearIntState(NFC_IRQ_GPIO,NFC_IRQ_PIN)  //清空GPIO中断信号

#define NFC_IRQ_INT_ENABLE()                                                 \
    {                                                                        \
        HAL_GPIO_EnableIRQ(NFC_IRQ_GPIO, NFC_IRQ_PIN, HAL_GPIO_IRQ_FALLING); \
        CLEAR_NFC_IRQ_STATE();                                               \
    }

#define NFC_IRQ_INT_DISABLE()                                                 \
    {                                                                        \
        HAL_GPIO_EnableIRQ(NFC_IRQ_GPIO, NFC_IRQ_PIN, HAL_GPIO_IRQ_NONE);   \
        CLEAR_NFC_IRQ_STATE();                                               \
    }

#define NFC_IRQ_INIT()                                                                            \
    {                                                                                             \
        HAL_GPIO_Init(NFC_IRQ_GPIO, NFC_IRQ_PIN, HAL_GPIO_MODE_INPUT_PULLUP, HAL_GPIO_PULL_HIGH); \
    }

/*****************Enum*****************/


/****************Struct****************/


/***************Variable***************/


/***************Function***************/
uint8_t nfc_hardware_init(void);

uint8_t NfcSetReg(uint8_t reg_addr, uint8_t reg_data);
uint8_t NfcGetReg(uint8_t reg_addr, uint8_t *reg_data);
uint8_t NfcModifyReg(uint8_t addr, uint8_t mask, uint8_t set);

uint8_t Write_Reg(uint8_t reg_addr, uint8_t reg_value);
uint8_t Read_Reg(uint8_t reg_addr);

uint8_t GetFIFOData(uint8_t addr, uint8_t *reg_data, uint8_t size);
uint8_t SetFIFOData(uint8_t addr, uint8_t *reg_data, uint8_t size);

unsigned char Set_BitMask(unsigned char reg_add,unsigned char mask);

void mDelay(uint16_t ms);

/**************************************/

#endif 

