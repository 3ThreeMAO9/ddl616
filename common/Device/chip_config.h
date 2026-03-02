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

#define _USER_OPTION_ADDR       (0x00100410)
#define _USER_OPTION_MAX_SIZE   (496)

#define OTA_FILE_HEADER_SIZE  (16)

/* bootloader configuration */
#define BOOTLOADER_BEGIN_ADDR (0xF000)
#define BOOTLOADER_TOTAL_SIZE (0x1000)

#define BOOTLOADER_CODE_ADDR  (BOOTLOADER_BEGIN_ADDR)
#define BOOTLOADER_CODE_SIZE  (0xFD00)

#define BOOTLOADER_INTR_ADDR  (0xFD40)

#define BOOTLOADER_USER_OPTION_ADDR (0xFE00)

/* application configuration */
#define FLASH_APP_BEGIN_ADDR  (0x0)
#define FLASH_APP_MAX_SIZE    (60 * 1024)   // 2 * 1024用于存放参数

#define FLASH_APP_HEADER_ADDR \
    (FLASH_APP_BEGIN_ADDR + FLASH_APP_MAX_SIZE - OTA_FILE_HEADER_SIZE)

#define SFLASH_BEGIN_ADDR (0x0)
#define SFLASH_MAX_SIZE   (4 * 1024 * 1024)
#define SFLASH_END_ADDR   (SFLASH_BEGIN_ADDR + SFLASH_MAX_SIZE)

/* configuration of ota partition in spi flash */
#define SFLASH_OTA_MAX_SIZE   (_ROM_MAX_SIZE)
#define SFLASH_OTA_BEGIN_ADDR (SFLASH_END_ADDR - SFLASH_OTA_MAX_SIZE)

/* configuration of backup partition in spi flash */
#define SFLASH_BACKUP_MAX_SIZE   (_ROM_MAX_SIZE)
#define SFLASH_BACKUP_BEGIN_ADDR (SFLASH_OTA_BEGIN_ADDR - SFLASH_BACKUP_MAX_SIZE)

#define SFLASH_HEADER_MAX_SIZE  (4 * 1024)
#define SFLASH_HEADER_ADDR      (SFLASH_END_ADDR - SFLASH_HEADER_MAX_SIZE)

#endif // CHIP_CONFIG_H
