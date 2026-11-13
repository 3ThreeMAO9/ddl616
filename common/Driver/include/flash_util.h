/**
 * @file flash_util.h
 * @author Dandjinh Deng (Dandjinh_Deng@on-bright.com)
 * @brief 
 * @version 0.1
 * @date 2022-10-11
 * 
 * @copyright Copyright (c) 2022 广州昂宝电子有限公司
 * 
 */

#ifndef BL_UTIL_H
#define BL_UTIL_H

#include <stdint.h>

/* 返回 0 = 擦除/编程/重载全部成功，1 = 任一环节超时失败 */
uint32_t flash_spi_copy_to_fmc(uint32_t spi_addr, uint32_t fmc_addr, uint32_t size);
void flash_fmc_copy_to_spi(uint32_t fmc_addr, uint32_t spi_addr, uint32_t size);

#endif // BL_UTIL_H
