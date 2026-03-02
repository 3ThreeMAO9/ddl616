#include "hal_fmc.h"


#define OB_LOG_LEVEL OB_LOG_LEVEL_NONE
#include "ob_log.h"
#define TAG "hal_fmc"

/***************Variable***************/


// ------------------------------------------

void hal_fmc_erase(uint32_t addr)
{
    if (((addr >= FLASH_USER_DEFINE_START_PAGE) && (addr < FLASH_USER_DEFINE_END_PAGE)) 
        || (addr >= FMC_SLEF_DEFINE_AREA))
    {
        FMC_PageErase(addr);
        OB_LOGI(TAG, "[%s] addr %08X", __func__, addr);
    }
    else
    {
        OB_LOGE(TAG, "Error[%s]", "addr is out");
    }
}

void hal_fmc_read(uint32_t addr, uint8_t* pData, uint16_t len)
{
    if (((addr >= FLASH_USER_DEFINE_START_PAGE) && (addr < FLASH_USER_DEFINE_END_PAGE)) 
        || (addr >= FMC_SLEF_DEFINE_AREA))
    {
        FMC_Read(addr, pData, len);
        OB_LOGI(TAG, "[%s] addr %08X", __func__, addr);
        OB_LOGI_DUMP(pData, len);
    }
    else
    {
        OB_LOGE(TAG, "Error[%s]", "addr is out");
    }
}

void hal_fmc_write(uint32_t addr, uint8_t* pData, uint16_t len)
{
    if (((addr >= FLASH_USER_DEFINE_START_PAGE) && (addr < FLASH_USER_DEFINE_END_PAGE)) 
        || (addr >= FMC_SLEF_DEFINE_AREA))
    {
        FMC_Write(addr, pData, len);
        OB_LOGI(TAG, "[%s] addr %08X", __func__, addr);
        OB_LOGI_DUMP(pData, len);
    }
    else
    {
        OB_LOGE(TAG, "Error[%s]", "addr is out");
    }
}
