#include "flash.h"

#define OB_LOG_LEVEL OB_LOG_LEVEL_NONE
#include "ob_log.h"
#define TAG "flash_data"

/***************Variable***************/
static flash_callback_t flash_callback;

// ------------------------------------------
uint8_t flash_erase_sectors(uint32_t addr, uint16_t cnt) {
    uint16_t i;

    for (i=0; i<cnt; i++){
        hal_fmc_erase(addr + (FLASH_SECTOR_SIZE * i));
    }
	return true;
}

void flash_erase_write_sector(uint32_t addr, uint8_t* pData, uint16_t size) {
    hal_fmc_erase(addr);
    hal_fmc_write(addr, pData, size);
}

uint8_t flash_read_sector(uint32_t addr, uint8_t* pData, uint16_t size) {
    hal_fmc_read(addr, pData, size);
	return true;
}

uint8_t flash_write_sector(uint32_t addr, uint8_t* pData, uint16_t size) {
    hal_fmc_write(addr, pData, size);
	return true;
}

void flash_area_init(flash_callback_t callback) {
	uint8_t i;
    OB_LOGD(TAG, "Init[%s]", "flash data area");

    flash_callback = callback;
    if (NULL == flash_callback) {
        return;
    }

    for (i = 0; i < FLASH_AREA_CNT; i++) {
        flash_callback(i);
    }
}

uint8_t flash_area_reset(void) {
    uint8_t sector_cnt;

    sector_cnt = (FLASH_USER_DEFINE_END_PAGE - FLASH_USER_DEFINE_START_PAGE + 1) / FLASH_SECTOR_SIZE;
    flash_erase_sectors(FLASH_USER_DEFINE_START_PAGE, sector_cnt);
	return true;
}

// ======================= end ===============================
