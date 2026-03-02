#include "parameter_manage.h"
#include "dual_sector.h"
#include "flash.h"

#define OB_LOG_LEVEL OB_LOG_LEVEL_NONE
#include "ob_log.h"
#define TAG "user_manage"

/***************Variable***************/
static dual_sector_manager_t parameter_manager;
static const flash_ops_t parameter_flash_ops = {
        .erase = flash_erase_sectors,
        .write = flash_write_sector,
        .read = flash_read_sector,
        .sector_a_addr = PARAMETER_SECTOR_START_ADDR,
        .sector_b_addr = PARAMETER_SECTOR_BACKUP_ADDR,
        .sector_cnt = PARAMETER_SECTOR_CNT,
};

// ------------------------------------------
void parameter_manage_init(void) {
    OB_LOGD(TAG,"parameter addr[%08X, %08X]", parameter_flash_ops.sector_a_addr, parameter_flash_ops.sector_b_addr);
    dual_sector_init(&parameter_manager, &parameter_flash_ops);

}

uint8_t write_parameter_info(const uint8_t* pData, uint16_t size) {
    return dual_sector_save_data(&parameter_manager, &parameter_flash_ops, (uint8_t*)pData, size);
}

uint8_t read_parameter_info(void* pData, uint16_t size) {
    return dual_sector_read_data(&parameter_manager, &parameter_flash_ops, pData, size);
}
