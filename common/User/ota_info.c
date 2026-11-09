/**
 * @file ota_info.c
 * @author Dandjinh Deng (Dandjinh_Deng@on-bright.com)
 * @brief 
 * @version 0.1
 * @date 2022-10-11
 * 
 * @copyright Copyright (c) 2022 广州昂宝电子有限公司
 * 
 */

#include "ota_info.h"
#include "chip_config.h"
#include <stddef.h>

#define OTA_INFO_ADDR _STACK_BOT_ADDR

static uint16_t ota_info_calc_checksum(ota_info_t *info)
{
#define _OTA_INFO_SUM_SIZE(_info) \
    (sizeof(*_info) - sizeof(_info->checksum))

    uint8_t *p = (uint8_t *)info;
    uint16_t i, sum;
    for (i = sum = 0; i < _OTA_INFO_SUM_SIZE(info); i++)
    {
        sum += *p;
        p++;
    }

    return sum;
}

ota_info_t *ota_info_get(uint32_t flag_check)
{
    ota_info_t *info = (ota_info_t *)OTA_INFO_ADDR;

    if (!flag_check)
        return info;

    if (info->magic != OTA_INFO_MAGIC)
        return NULL;

    if (info->checksum != ota_info_calc_checksum(info))
        return NULL;

    return info;
}

void ota_info_update(ota_info_t *info)
{
    info->magic = OTA_INFO_MAGIC;
    uint16_t sum = ota_info_calc_checksum(info);
    info->checksum = sum;
}
