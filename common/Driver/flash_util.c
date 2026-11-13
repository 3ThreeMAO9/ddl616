/**
 * @file flash_util.c
 * @author Dandjinh Deng (Dandjinh_Deng@on-bright.com)
 * @brief 
 * @version 0.1
 * @date 2022-10-11
 * 
 * @copyright Copyright (c) 2022 广州昂宝电子有限公司
 * 
 */

#include "flash_util.h"
#include "boot_spi_flash.h"
#include "fmc_flash.h"
#include "ota_file.h"
#include "main.h"

#if defined (__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050)
    #pragma clang diagnostic push
    #pragma clang diagnostic ignored "-Wcast-align"
#endif

uint32_t flash_spi_copy_to_fmc(uint32_t spi_addr, uint32_t fmc_addr, uint32_t size)
{
    uint32_t n;

    if (fmc_flash_erase_pages(fmc_addr, fmc_addr + size))
        return 1;

    while (size)
    {
        n = (size > g_boot_buffer_size) ? g_boot_buffer_size : size;
        spi_flash_read(spi_addr, g_boot_buffer, n);
        if (fmc_flash_program(fmc_addr, (uint32_t *)g_boot_buffer, n))
            return 1;

        spi_addr += n;
        fmc_addr += n;
        size -= n;
    }

    return fmc_flash_reload();
}

#if defined (__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050)
    #pragma clang diagnostic pop
#endif

void flash_fmc_copy_to_spi(uint32_t fmc_addr, uint32_t spi_addr, uint32_t size)
{
#define _SFLASH_ERASE_SIZE (64 * 1024)

    uint32_t n = size;
    uint32_t addr = spi_addr;
    while (1)
    {
        spi_flash_64k_erase(addr);
        if (n > _SFLASH_ERASE_SIZE)
        {
            addr += _SFLASH_ERASE_SIZE;
            n -= _SFLASH_ERASE_SIZE;
            continue;
        }
        
        break;
    }

    uint8_t *data = (uint8_t *)fmc_addr;
    addr = spi_addr;
    while (size)
    {
        n = (size > SPI_FLASH_PAGE_SIZE) ? SPI_FLASH_PAGE_SIZE : size;
        spi_flash_write(addr, data, n);

        addr += n;
        data += n;
        size -= n;
    }
}
