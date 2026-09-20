#include "flash_drive.h"
#include "bsp_rom_config.h"

#define OB_LOG_LEVEL OB_LOG_LEVEL_NONE
#include "ob_log.h"
#define TAG "flash_drive"

/***************Variable***************/


// ------------------------------------------
uint32_t user_flash_erase(uint32_t sector_addr, uint32_t size)
{
    sector_addr = sector_addr / FLASH_ERASE_SIZE * FLASH_ERASE_SIZE;      //对地址取整

    if ((sector_addr >= FLASH_USER_DEFINE_START_ADDR) && (sector_addr < FLASH_USER_DEFINE_END_ADDR))
    {
        return hal_flash_erase(sector_addr, size);
    }

#if (Enabled == PRINTF_ERR)
    OB_LOGE(TAG, "Err: fmc addr[%08X] is out", sector_addr);
#endif
    return SFUD_ERR_ADDR_OUT_OF_BOUND;
}

uint32_t user_flash_write(uint32_t addr, const void *buf, uint32_t size)
{
    if ((addr >= FLASH_USER_DEFINE_START_ADDR) && (addr < FLASH_USER_DEFINE_END_ADDR))
    {
        return hal_flash_write(addr, buf, size);
    }

#if (Enabled == PRINTF_ERR)
    OB_LOGE(TAG, "Err: fmc addr[%08X] is out", addr);
#endif

    return SFUD_ERR_ADDR_OUT_OF_BOUND;
}

uint32_t user_flash_read(uint32_t addr, void *buf, uint32_t size)
{
    return hal_flash_read(addr, buf, size);
}

void flash_read_data_block(uint32_t pageAddr, uint16_t index, uint8_t* pData, uint16_t blockSize)
{
    user_flash_read((pageAddr + (blockSize * index)), pData, blockSize);
}

void flash_write_data_pages(uint32_t pageAddr, uint32_t backupAddr, uint8_t* pData, uint16_t size)
{
    const uint32_t flag = true;

    //first write backup page
    user_flash_erase(backupAddr, size);

    user_flash_write(backupAddr + sizeof(uint32_t), pData, size);
    user_flash_write(backupAddr, (uint8_t*)(&flag), sizeof(uint32_t));

    //first write backup page
    user_flash_erase(pageAddr, size);

    user_flash_write(pageAddr + sizeof(uint32_t), pData, size);
    user_flash_write(pageAddr, (uint8_t*)(&flag), sizeof(uint32_t));
}

void flash_read_data_pages(uint32_t pageAddr, uint8_t* pData, uint16_t size)
{
    user_flash_read(pageAddr + sizeof(uint32_t), pData, size);
}
