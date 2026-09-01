#include "bsp_hmi.h"

/***************Variable***************/
const bsp_hmi_drive_io_t bsp_hmi = {
    .init = module_hmi_init,
    .loop = module_hmi_loop,
    .setState = module_hmi_handle,
    .config = module_hmi_config,
};

// ------------------------------------------

const bsp_hmi_drive_io_t* bsp_hmi_get_driver(void)
{
    return &bsp_hmi;
}
