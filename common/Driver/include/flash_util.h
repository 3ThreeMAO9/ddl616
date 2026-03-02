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
#include "ota_file.h"

struct copy_to_fmc_param_t_def {
    uint8_t *buffer;
    uint32_t size;
    uint32_t spi_addr;
};
typedef struct copy_to_fmc_param_t_def copy_to_fmc_param_t;

void flash_spi_copy_to_fmc(copy_to_fmc_param_t *param);
void flash_fmc_copy_to_spi(uint32_t spi_addr);

#endif // BL_UTIL_H
