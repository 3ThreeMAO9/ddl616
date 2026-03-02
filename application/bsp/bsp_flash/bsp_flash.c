#include "bsp_flash.h"

/***************Variable***************/
const bsp_flash_drive_io_t bsp_flash = {
    .init = flash_area_init,
    .erase_write = flash_erase_write_sector,
    .erase_sectors = flash_erase_sectors,
    .write_sectors = flash_write_sector,
    .read_sectors = flash_read_sector,
};

// ------------------------------------------

const bsp_flash_drive_io_t* bsp_flash_get_driver(void){
    return &bsp_flash;
}
