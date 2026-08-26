/**
 * @file hal_flash.h
 * @author Dandjinh Deng (Dandjinh_Deng@on-bright.com)
 * @brief 
 * @version 0.1
 * @date 2023-02-22
 * 
 * @copyright Copyright (c) 2023 广州昂宝电子有限公司
 * 
 */

#ifndef _HAL_FLASH_H
#define _HAL_FLASH_H

#include <stdint.h>
#include <sfud.h>
#include "sfud_cfg.h"
#include <string.h>

#define HAL_FLASH_SECTOR_SIZE (4096)

#define HAL_FLASH_BEGIN_ADDR (0x0)
#define HAL_FLASH_TOTAL_SIZE (512 * 1024)
#define HAL_FLASH_END_ADDR   (HAL_FLASH_BEGIN_ADDR + HAL_FLASH_TOTAL_SIZE)

#define HAL_FLASH_RW_ALIGN (4)

uint32_t hal_flash_erase(uint32_t sector_addr, uint32_t size);

uint32_t hal_flash_write(uint32_t addr, const void *buf, uint32_t size);

uint32_t hal_flash_read(uint32_t addr, void *buf, uint32_t size);

#endif // _HAL_FLASH_H
