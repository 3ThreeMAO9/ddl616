/**
 * Copyright(C) 2024 GZ-OB. All Rights Reserved
 * File: falsh_data.h
 * Desc: 
 * 最小擦写单位：FLASH_SECTOR_SIZE(大小取决于是页操作还是扇区操作)；
 * 最大数据块大小为DATA_BLOCK_SIZE（Flash临时分配的缓存数据块的堆栈，因此需不小于项目中用到的最大数据块大小）；
 * 数据片的首字节为块标识(枚举：block_flag_t)，字节对齐，从首地址后移一个实际数据块大小为数据内容；
 * BLOCK_EMPTY-块空闲；BLOCK_VALID-块已写入数据；BLOCK_INVALID-块数据被删除；
 * Ver : V1.00
 * Revision: Ren
 * Date: 2025-08-16
 *  */
#ifndef FLASH__HH
#define FLASH__HH

#include "config.h"
#include "hal_fmc.h"

typedef void (*flash_callback_t)(uint8_t);

/*****************Macro****************/
#define SECTOR_DATA_VALID_FLAG                      (0x01)

/*****************Enum*****************/
typedef enum{
    BLOCK_INVALID = 0,
    BLOCK_VALID,

    BLOCK_FREE = 0xFF,

}block_flag_e;

typedef enum{
    FLASH_AREA_PARAMETER = 0,
    
    FLASH_AREA_CNT,
}flash_area_index_e;


/****************Struct****************/
#pragma pack(1)


#pragma pack()




/***************Variable***************/


/***************Function***************/
void flash_write_data_block(uint32_t addr, uint16_t index, uint8_t* pData, const uint16_t block_size);
void flash_read_data_block(uint32_t addr, uint16_t index, uint8_t* pData, const uint16_t block_size);

void flash_modify_data_block(uint32_t sector_addr, uint32_t backup_addr, uint8_t sector_cnt, uint16_t index, uint8_t* pData, uint16_t block_size);
void flash_area_renew(uint32_t sector_addr, uint32_t backup_addr, uint8_t sector_cnt, uint16_t index, uint8_t* pData, uint16_t block_size);

void flash_erase_write_sectors_safe(uint32_t sector_addr, uint32_t backup_addr, uint8_t* pData, uint16_t size);
void flash_erase_write_sector(uint32_t addr, uint8_t* pData, uint16_t size);

uint8_t flash_read_sector(uint32_t addr, uint8_t* pData, uint16_t size);
uint8_t flash_write_sector(uint32_t addr, uint8_t* pData, uint16_t size);
uint8_t flash_erase_sectors(uint32_t addr, uint16_t cnt);

uint8_t is_lose_sector_data(uint32_t sector_addr);
void flash_write_sector_flag(uint32_t addr);

void flash_area_init(flash_callback_t callback);
uint8_t flash_area_reset(void);

/**************************************/

#endif 
