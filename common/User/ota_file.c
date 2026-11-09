/**
 * @file ota_file.c
 * @author Dandjinh Deng (Dandjinh_Deng@on-bright.com)
 * @brief 
 * @version 0.1
 * @date 2022-08-31
 * 
 * @copyright Copyright (c) 2022 广州昂宝电子有限公司
 * 
 */

#include "ota_file.h"
#if !defined(__linux) && !defined(_WIN32)
#include "boot_spi_flash.h"
#include "main.h"
#endif

#include <string.h>
#include <stddef.h>

#define CHECKSUM_U16_MASK ((uint16_t)~0x01)
#define CHECKSUM_U32_MASK ((uint32_t)~0x03)

uint8_t *g_boot_buffer;
uint32_t g_boot_buffer_size;

uint16_t ota_file_calc_checksum_u16(uint16_t *ptr, uint32_t size)
{
    uint16_t sum;
    for (sum = 0; size & CHECKSUM_U16_MASK; ptr++)
    {
        sum += *ptr;
        size -= sizeof(*ptr);
    }

    if (size)
        sum += *ptr & 0xFF;

    return sum;
}

uint32_t ota_file_calc_checksum_u32(uint32_t *ptr, uint32_t size)
{
    uint32_t sum;
    for (sum = 0; size & CHECKSUM_U32_MASK; ptr++)
    {
        sum += *ptr;
        size -= sizeof(*ptr);
    }

    switch (size)
    {
    case 3:
        sum += *ptr & 0xFFFFFF;
        break;
    case 2:
        sum += *ptr & 0xFFFF;
        break;
    case 1:
        sum += *ptr & 0xFF;
        break;
    }

    return sum;
}

#if !defined(__linux) && !defined(_WIN32)
__attribute__((section("BL1")))
static void ota_file_read_flash(uint32_t addr, uint32_t flag_inner_flash,
    void *buf, uint32_t size)
{
    if (flag_inner_flash)
        memcpy(buf, (void *)addr, size);
    else
        spi_flash_read(addr, (uint8_t *)buf, size);
}

#if defined (__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050)
    #pragma clang diagnostic push
    #pragma clang diagnostic ignored "-Wcast-align"
#endif

uint32_t ota_file_validate(uint32_t addr,
    uint32_t flag_inner_flash, ota_file_t *out_file)
{
    uint32_t sum, *ptr, size, rd, cs2_addr;

    /* check header first */
    ota_file_read_flash(addr, flag_inner_flash, out_file, sizeof(*out_file)); 
    if ((out_file->magic != OTA_FILE_MAGIC)
        || (out_file->size < 4))
    {
        return 1;
    }

    sum = ota_file_calc_checksum_u16((uint16_t *)(&out_file->magic),
        offsetof(ota_file_t, data) - sizeof(out_file->checksum1));
    if (sum != out_file->checksum1)
        return 1;

    /* check content next */
    sum = 0;
    ptr = (uint32_t *)(g_boot_buffer);
    cs2_addr = addr + sizeof(*out_file);
    size = out_file->size;
    while (size)
    {
        /* rd shouble be 4 aligned unless the last bytes */
        rd = (size > g_boot_buffer_size) ? g_boot_buffer_size : size;
        ota_file_read_flash(cs2_addr, flag_inner_flash, ptr, rd);
        sum += ota_file_calc_checksum_u32(ptr, rd);
        size -= rd;
        cs2_addr += rd;
    }

    if (sum != out_file->checksum2)
        return 1;

    return 0;
}

#if defined (__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050)
    #pragma clang diagnostic pop
#endif

#endif
