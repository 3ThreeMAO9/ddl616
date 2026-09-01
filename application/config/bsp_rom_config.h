/**
 * @file bsp_rom_config.h
 * @author Dandjinh Deng (Dandjinh_Deng@on-bright.com)
 * @brief 
 * @version 0.1
 * @date 2023-02-21
 * 
 * @copyright Copyright (c) 2023 广州昂宝电子有限公司
 * 
 */

#ifndef _BSP_ROM_CONFIG_H
#define _BSP_ROM_CONFIG_H

#include "item_config.h"

//flash addr
#define FLASH_ERASE_SIZE                (0x1000)         //falsh erase size
#define FLASH_USER_DEFINE_START_ADDR    (0x1000)
#define FLASH_USER_DEFINE_END_ADDR      (0x17FFF)

//page cnt
#define SPI_FLASH_SECTOR_NUM            (1)
#define USER_PAGE_CNT                   (((USER_BLOCK_SIZE * USER_CNT) + FLASH_ERASE_SIZE) / FLASH_ERASE_SIZE) //(32*222+4096)/4096   2.73
#define PARAMETER_PAGE_CNT              (1)
#define PRODUCE_DATA_PAGE_CNT           (1)
#define LOCK_LOG_SECTOR_NUM             (2)
#define GC_SECTOR_CNT                   (1)
#define WAKEUP_STATE_CNT                (1)
#define ONE_LINE_KEY_CNT                (1)

//audio index addr
#define SPI_FLASH_INFO_BEGIN_ADDR       (FLASH_USER_DEFINE_START_ADDR)  //spi flash地址信息

//user addr
#define USER_PAGE_START_ADDR            (SPI_FLASH_INFO_BEGIN_ADDR + FLASH_ERASE_SIZE * SPI_FLASH_SECTOR_NUM)  //0x1000+0x1000=0x2000
#define USER_PAGE_BACKUP_ADDR           (USER_PAGE_START_ADDR + FLASH_ERASE_SIZE * USER_PAGE_CNT)   //0x4000

//parameter addr
#define PARAMETER_PAGE_START_ADDR       (USER_PAGE_BACKUP_ADDR + FLASH_ERASE_SIZE * USER_PAGE_CNT)  //0x6000
#define PARAMETER_PAGE_BACKUP_ADDR      (PARAMETER_PAGE_START_ADDR + FLASH_ERASE_SIZE * PARAMETER_PAGE_CNT) //0x7000

//produce data addr
#define PRODUCE_DATA_PAGE_START_ADDR    (PARAMETER_PAGE_BACKUP_ADDR + FLASH_ERASE_SIZE * PARAMETER_PAGE_CNT)    //0x8000

//log data addr
#define LOCK_LOG_FLASH_ADDR             (PRODUCE_DATA_PAGE_START_ADDR + FLASH_ERASE_SIZE * PRODUCE_DATA_PAGE_CNT)   //0x9000

//test data addr
#define GC_SECTOR_ADDR                  (LOCK_LOG_FLASH_ADDR + FLASH_ERASE_SIZE * LOCK_LOG_SECTOR_NUM)      //0xB000

//sleep wake state data addr
#define WAKEUP_STATE_ADDR               (GC_SECTOR_ADDR + FLASH_ERASE_SIZE * GC_SECTOR_CNT)                 //0xC000

//blue one line key data addr
#define ONE_LINE_KEY_ADDR               (WAKEUP_STATE_ADDR + FLASH_ERASE_SIZE * WAKEUP_STATE_CNT)           //0xD000


///END SPI FLASH


// chip flash  (0x7C000~0x7E000)
#define LOCK_LOG_CHIP_FLASH_ADDR        0x7C000 //(0x7C000~0x7E000)

#define BLE_PARAMETER_CHIP_FLASH_ADDR   0x7E000
#endif // _BSP_ROM_CONFIG_H
