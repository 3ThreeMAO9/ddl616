/**
 * @file fmc_flash.h
 * @author Dandjinh Deng (Dandjinh_Deng@on-bright.com)
 * @brief 
 * @version 0.1
 * @date 2022-09-14
 * 
 * @copyright Copyright (c) 2022 广州昂宝电子有限公司
 * 
 */

#ifndef FMC_FLASH_H
#define FMC_FLASH_H

#include <stdint.h>

#define FMC_BOOT_TO_APP (0)
#define FMC_BOOT_TO_OTA (1)

uint32_t fmc_read(uint32_t addr, uint32_t *buf, uint32_t size);

uint32_t fmc_flash_erase_pages(uint32_t begin_addr, uint32_t end_addr);

uint32_t fmc_flash_program(uint32_t addr, uint32_t *data, uint32_t size);

uint32_t fmc_flash_reload(void);

uint32_t fmc_flash_wait(void);

uint32_t fmc_get_boot_to(void);

void fmc_set_boot_to(uint32_t target);

#endif // FMC_FLASH_H
