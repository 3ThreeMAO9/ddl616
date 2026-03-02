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
#include "chip_config.h"

#if defined (__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050)
    #pragma clang diagnostic push
    #pragma clang diagnostic ignored "-Wcast-align"
#endif

void flash_spi_copy_to_fmc(copy_to_fmc_param_t *param)
{
    fmc_flash_erase_pages(FLASH_APP_BEGIN_ADDR,
        FLASH_APP_BEGIN_ADDR + FLASH_APP_MAX_SIZE);

    ota_file_t header;
    spi_flash_read(param->spi_addr, (uint8_t *)&header, sizeof(header));

    uint32_t spi_addr = param->spi_addr + sizeof(header);
    uint32_t fmc_addr = FLASH_APP_BEGIN_ADDR;

    uint32_t size = header.size;
    uint32_t rd;
    while (size) // copy firmware data
    {
        rd = (size > param->size) ? param->size : size;
        spi_flash_read(spi_addr, param->buffer, rd);
        fmc_flash_program(fmc_addr, (uint32_t *)param->buffer, rd);

        spi_addr += rd;
        fmc_addr += rd;
        size -= rd;
    }

    // copy header data
    fmc_flash_program(FLASH_APP_HEADER_ADDR,
        (uint32_t *)&header, OTA_FILE_HEADER_SIZE);

    fmc_flash_reload();
}

void flash_fmc_copy_to_spi(uint32_t spi_addr)
{
#define _SFLASH_ERASE_SIZE (64 * 1024)

    ota_file_t *header = (ota_file_t *)FLASH_APP_HEADER_ADDR;

    uint32_t size = sizeof(*header) + header->size;
    uint32_t addr = spi_addr;
    while (size)
    {
        spi_flash_64k_erase(addr);
        addr -= _SFLASH_ERASE_SIZE;
        if (size > _SFLASH_ERASE_SIZE)
            size -= _SFLASH_ERASE_SIZE;
        else
            size = 0;
    }

    addr = spi_addr;
    spi_flash_write(addr, (uint8_t *)header, sizeof(*header));
    addr += sizeof(*header);

    size = header->size;
    uint8_t *data = (uint8_t *)FLASH_APP_BEGIN_ADDR;
    uint32_t wr = SPI_FLASH_PAGE_SIZE - sizeof(*header);
    spi_flash_write(addr, data, wr);
    addr += wr;
    data += wr;
    size -= wr;

    while (size)
    {
        wr = (size > SPI_FLASH_PAGE_SIZE) ? SPI_FLASH_PAGE_SIZE : size;
        spi_flash_write(addr, data, wr);
        addr += wr;
        data += wr;
        size -= wr;
    }
}

#if defined (__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050)
    #pragma clang diagnostic pop
#endif
