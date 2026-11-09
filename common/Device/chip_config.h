/**
 * @file chip_config.h
 * @author Dandjinh Deng (Dandjinh_Deng@on-bright.com)
 * @brief 
 * @version 0.1
 * @date 2022-10-11
 * 
 * @copyright Copyright (c) 2022 广州昂宝电子有限公司
 * 
 */

#ifndef CHIP_CONFIG_H
#define CHIP_CONFIG_H

#define _ROM_BEGIN_ADDR (0x0000)
#define _ROM_MAX_SIZE   (64 * 1024)

#define _RAM_BEGIN_ADDR (0x20000000)
#define _RAM_MAX_SIZE   (8 * 1024)

#define _ROM_PAGE_SIZE  (512)

/* bootloader total size */
#define _BL_TOTAL_SIZE   (4 * _ROM_PAGE_SIZE)
/* bootloader part1 configuration*/
#define _BL1_BEGIN_ADDR  (_ROM_BEGIN_ADDR)
#define _BL1_MAX_SIZE    (2 * _ROM_PAGE_SIZE)
/* bootloader part2 configuration*/
#define _BL2_MAX_SIZE    (_BL_TOTAL_SIZE - _BL1_MAX_SIZE)
#define _BL2_BEGIN_ADDR  (_ROM_BEGIN_ADDR + _ROM_MAX_SIZE - _BL2_MAX_SIZE)
/* size of bootloader export variables*/
#define _BL_RAM_SIZE    (16)

/* stack configuration */
#define _STACK_MAX_SIZE (0x800)
#define _STACK_TOP_ADDR (_RAM_BEGIN_ADDR + _RAM_MAX_SIZE)
#define _STACK_BOT_ADDR (_STACK_TOP_ADDR - _STACK_MAX_SIZE)

/* application configuration */
#define FLASH_APP_BEGIN_ADDR  (_BL1_BEGIN_ADDR + _BL1_MAX_SIZE)
#define FLASH_APP_MAX_SIZE    (_ROM_MAX_SIZE - _BL_TOTAL_SIZE)

#define SFLASH_BEGIN_ADDR (0x0U)
#define SFLASH_MAX_SIZE   (4 * 1024 * 1024)
//#define SFLASH_MAX_SIZE   (16 * 1024 * 1024)
#define SFLASH_END_ADDR   (SFLASH_BEGIN_ADDR + SFLASH_MAX_SIZE)

/* configuration of ota partition in spi flash */
#define SFLASH_OTA_MAX_SIZE   (_ROM_MAX_SIZE)
#define SFLASH_OTA_BEGIN_ADDR (SFLASH_END_ADDR - SFLASH_OTA_MAX_SIZE)

/* configuration of backup partition in spi flash */
#define SFLASH_BACKUP_MAX_SIZE   (_ROM_MAX_SIZE)
#define SFLASH_BACKUP_BEGIN_ADDR (SFLASH_OTA_BEGIN_ADDR - SFLASH_BACKUP_MAX_SIZE)

/*****************  user-defined addr  ***************/
#define FMC_PAGE_SIZE                               (0x200)
#define FMC_SLEF_DEFINE_AREA                        (0x00100410)

#define FLASH_SECTOR_SIZE                           (FMC_PAGE_SIZE)

/* 仅 hal_fmc 的地址范围判断在使用；参数已全部迁到 SPI flash */
#define FLASH_USER_DEFINE_START_PAGE                (0xEA00)
#define FLASH_USER_DEFINE_END_PAGE                  (0xEFFF)

#endif // CHIP_CONFIG_H
