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
#include "chip_config.h"
#include "fmc.h"
#include "ob_log.h"

#include <string.h>
#include <stddef.h>

#define CHECKSUM_U16_MASK ((uint16_t)~0x01)
#define CHECKSUM_U32_MASK ((uint32_t)~0x03)

extern const uint32_t g_vector_table_size;

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

static uint32_t ota_file_calc_vector_table_checksum(uint8_t *buffer, uint32_t size)
{
    uint32_t sum = 0;
    uint32_t addr = FLASH_APP_BEGIN_ADDR;
    uint32_t len = g_vector_table_size / 4;
    uint32_t *ptr = (uint32_t *)buffer;
    uint32_t rd;
    size /= 4;
    while (len)
    {
        rd = (len > size) ? size : len;
        FMC_read(addr, ptr, rd);
        sum += ota_file_calc_checksum_u32(ptr, rd * 4);

        addr += rd * 4;
        len -= rd;
    }

    return sum;
}

uint32_t ota_file_check_inner_flash(uint8_t *buffer, uint32_t size)
{
    ota_file_t *ptr_file = (ota_file_t *)FLASH_APP_HEADER_ADDR;
    if ((ptr_file->magic != OTA_FILE_MAGIC) ||
        (ptr_file->size < 4))
    {
        OB_LOGD("inH");
        return 1;
    }

    uint16_t sum1 = ota_file_calc_checksum_u16((uint16_t *)(&ptr_file->magic),
        offsetof(ota_file_t, data) - sizeof(ptr_file->checksum1));
    if (sum1 != ptr_file->checksum1)
    {
        OB_LOGD("inS1");
        return 1;
    }

    uint32_t sum2;
    if (FLASH_APP_BEGIN_ADDR < g_vector_table_size)
        sum2 = ota_file_calc_vector_table_checksum(buffer, size);
    else
        sum2 = 0;

    sum2 += ota_file_calc_checksum_u32(
        (uint32_t *)g_vector_table_size, ptr_file->size - g_vector_table_size);
    if (sum2 != ptr_file->checksum2)
    {
        OB_LOGD("inS2");
        return 1;
    }

    OB_LOGD("inOK");
    return 0;
}

uint32_t ota_file_check_outter_flash(ota_file_check_param_t *param)
{
    ota_file_t *out_file = &param->out_file;
    FMC_Read(param->addr, (uint32_t *)out_file, sizeof(*out_file));

    if ((out_file->magic != OTA_FILE_MAGIC)
        || (out_file->size < 4))
    {
        OB_LOGD("outH");
        return 1;
    }

    uint16_t sum1 = ota_file_calc_checksum_u16((uint16_t *)(&out_file->magic),
        offsetof(ota_file_t, data) - sizeof(out_file->checksum1));
    if (sum1 != out_file->checksum1)
    {
        OB_LOGD("outS1");
        return 2;
    }

    uint32_t sum, *ptr, size, rd, cs2_addr;
    /* check content next */
    sum = 0;
    ptr = (uint32_t *)(param->buffer);
    cs2_addr = param->addr + sizeof(*out_file);
    size = out_file->size;
    while (size)
    {
        /* rd shouble be 4 aligned unless the last bytes */
        rd = (size > param->size) ? param->size : size;
        
        FMC_Read(cs2_addr, param->buffer, rd);
        sum += ota_file_calc_checksum_u32(ptr, rd);
        size -= rd;
        cs2_addr += rd;
    }

    if (sum != out_file->checksum2)
    {
        OB_LOGD("outS2");
        return 3;
    }

    OB_LOGD("outOK");
    return 0;
}

uint32_t ota_sflash_header_check(ota_file_check_param_t *param)
{
    ota_file_t *out_file = &param->out_file;
    uint16_t sum1 = 0;

    for (uint8_t i = 0; i < 3; i++)
    {
        FMC_Read(param->addr, (uint8_t *)out_file, sizeof(ota_file_t));

        sum1 = ota_file_calc_checksum_u16((uint16_t *)(&out_file->magic),
                                          sizeof(ota_file_t) - sizeof(uint16_t));
        if (sum1 == out_file->checksum1)
        {
            break;
        }
    }

    if (sum1 != out_file->checksum1)
    {
        OB_LOGD("outS1");
        return 1;
    }

    if ((out_file->magic != OTA_FILE_MAGIC) || (out_file->size < 4))
    {
        OB_LOGD("outH");
        return 2;
    }

    OB_LOGD("ota_sflash_header_check OK");
    return 0;
}
void ota_sflash_header_set(uint32_t addr, uint16_t magic)
{
    ota_file_t out_file;
    memset(&out_file, 0xFF, sizeof(ota_file_t));
    out_file.magic = magic;
    out_file.checksum1 = ota_file_calc_checksum_u16((uint16_t *)(&out_file.magic),
                                                    sizeof(ota_file_t) - sizeof(uint16_t));

    FMC_PageErase(addr);
    FMC_Read(addr, (uint8_t *)&out_file, sizeof(ota_file_t));
}
