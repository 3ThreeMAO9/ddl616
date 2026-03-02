#include "task_flash.h"

#include "user_parameter.h"

#define OB_LOG_LEVEL OB_LOG_LEVEL_NONE
#include "ob_log.h"
#define TAG "task_flash"

/***************Variable***************/
static flash_task_driver_t flash_task_driver;

static void flash_task_callback(uint8_t flash_area) {

    OB_LOGD(TAG, "flash_area[%u]", flash_area);
    switch (flash_area) {
        case FLASH_AREA_PARAMETER:
            user_parameter_init();
            break;
        default:
            break;
    }
}

void flash_task_init(void) {
    OB_LOGD(TAG, "Init");

    flash_task_driver.io = bsp_flash_get_driver();
    if (NULL != flash_task_driver.io->init){
        flash_task_driver.io->init(flash_task_callback);
    }
}
