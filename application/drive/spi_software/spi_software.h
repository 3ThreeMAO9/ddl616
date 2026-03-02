/**
 * Copyright(C) 2024 GZ-OB. All Rights Reserved
 * File: spi_software.h
 * Desc: 
 * Ver : V1.00
 * Revision: Ren
 * Date: 2025-09-15
 *  */

#ifndef __SPI_SOFTWARE_H__
#define __SPI_SOFTWARE_H__

#include <stdint.h>
#include <stdbool.h>
#include "config.h"
#include "hal_gpio.h"

/*****************Macro****************/


/*****************Enum*****************/

/****************Struct****************/
// GPIO操作函数结构体
typedef struct {
    void (*init)(void);                 // 硬件IO初始化
    void (*set_cs)(bool high);          // 设置CS引脚电平
    void (*set_clk)(bool high);         // 设置CLK引脚电平
    void (*set_mosi)(bool high);        // 设置MOSI引脚电平
    bool (*get_miso)(void);             // 获取MISO引脚电平
} SPI_GPIO_Ops;

// I2C配置结构体
typedef struct {
    SPI_GPIO_Ops ops;               // GPIO操作函数

} SPI_Config;


/***************Variable***************/


/***************Function***************/
// ========== 基础API ==========
uint8_t SPI_Init(const SPI_Config *config);
uint8_t SPI_TransferByte(const SPI_Config *config, uint8_t w_byte);

// ========== 高级API ==========
uint8_t SPI_WriteReg(const SPI_Config *config, uint8_t reg_addr, uint8_t w_byte);
uint8_t SPI_ReadReg(const SPI_Config *config, uint8_t reg_addr, uint8_t* r_byte);

/**************************************/

#endif 
