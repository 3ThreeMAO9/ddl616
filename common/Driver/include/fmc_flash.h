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

#define FMC_ISP_PROGRAM     (0x02 << 0x04)
#define FMC_ISP_PAGE_ERASE  (0x04 << 0x04)
#define FMC_ISP_CHIP_RELOAD (0x0F << 0x04)

#define FMC_FLASH_PAGE_SIZE (512)

uint32_t fmc_flash_erase_pages(uint32_t begin_addr, uint32_t end_addr);

uint32_t fmc_flash_program(uint32_t addr, uint32_t *data, uint32_t size);

uint32_t fmc_flash_reload(void);

uint32_t fmc_flash_wait(void);

#endif // FMC_FLASH_H
