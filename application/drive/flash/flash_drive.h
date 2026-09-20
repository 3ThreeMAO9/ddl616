/**
 * Copyright(C) 2024 GZ-OB. All Rights Reserved
 * File: flash_drive.h
 * Desc: Flash支持扇区擦除(4096 Bytes)；
 *       每页4096 Bytes(FLASH_ERASE_SIZE)，每个块预分配32Bytes（DATA_BLOCK_SIZE），每页划分为128块(PER_PAGE_DATA_BLOCK_CNT)；
 *       首字节为块标识(枚举：block_flag_t)，BLOCK_EMPTY-块空闲；BLOCK_VALID-块已写入数据；BLOCK_INVALID-块数据被删除；
 *       备份页（USER_PAGE_BACKUP_ADDR）用于当没有空闲块时，块回收缓存；
 * Ver : V1.00
 * Revision: Ren
 * Date: 2025-03-20
 *  */
#ifndef FLASH_DRIVE__HH
#define FLASH_DRIVE__HH

#include "config.h"
#include "hal_flash.h"

/*****************Macro****************/
#define PER_PAGE_DATA_BLOCK_CNT                     (FLASH_ERASE_SIZE / USER_BLOCK_SIZE)
#define DATA_BLOCK_CNT                              (PER_PAGE_DATA_BLOCK_CNT * USER_PAGE_CNT)

/*****************Enum*****************/
typedef enum{
    BLOCK_INVALID = 0,
    BLOCK_VALID,

    BLOCK_FREE = 0xFF,

}block_flag_t;

typedef enum{
    FMC_PAGE_USER = 0,
    FMC_PAGE_PARAMETER,

    FMC_PAGE_CNT,

}fmc_page_index_t;

/****************Struct****************/
#pragma pack(1)


#pragma pack()


/***************Variable***************/


/***************Function***************/
uint32_t user_flash_erase(uint32_t sector_addr, uint32_t size);
uint32_t user_flash_write(uint32_t addr, const void *buf, uint32_t size);
uint32_t user_flash_read(uint32_t addr, void *buf, uint32_t size);

void flash_read_data_block(uint32_t pageAddr, uint16_t index, uint8_t* pData, uint16_t size);

void flash_write_data_pages(uint32_t pageAddr, uint32_t backupAddr, uint8_t* pData, uint16_t size);
void flash_read_data_pages(uint32_t pageAddr, uint8_t* pData, uint16_t size);

/**************************************/

#endif 
