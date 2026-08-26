/**
 * @file hal_flash.c
 * @author Dandjinh Deng (Dandjinh_Deng@on-bright.com)
 * @brief 
 * @version 0.1
 * @date 2023-02-22
 * 
 * @copyright Copyright (c) 2023 广州昂宝电子有限公司
 * 
 */

#include "hal_flash.h"
#include "flash.h"

#define OB_LOG_LEVEL OB_LOG_LEVEL_NONE
#include "ob_log.h"
#define TAG "hal_flash"

uint32_t hal_flash_erase(uint32_t sector_addr, uint32_t size)
{
    const sfud_flash* flash = sfud_get_device_table() + 0;
    OB_LOGI(TAG, "[%s] addr %08X", __func__, sector_addr);
    return sfud_erase(flash, sector_addr, size);
}

uint32_t hal_flash_write(uint32_t addr, const void *buf, uint32_t size)
{
    const sfud_flash* flash = sfud_get_device_table() + 0;
    OB_LOGI(TAG, "[%s] addr %08X", __func__, addr);
    return sfud_write(flash,addr,size,buf);
}

uint32_t hal_flash_read(uint32_t addr, void *buf, uint32_t size)
{
    const sfud_flash *flash = sfud_get_device_table() + 0;
    OB_LOGI(TAG, "[%s] addr %08X", __func__, addr);
    sfud_read(flash, addr, size, buf);
    return 0;
}
