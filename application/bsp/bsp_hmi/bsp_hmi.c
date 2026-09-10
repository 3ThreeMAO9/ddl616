#include "bsp_hmi.h"

/***************Variable***************/
const bsp_hmi_drive_io_t bsp_hmi = {
    .init = module_hmi_init,
    .loop = module_hmi_loop,
    .setState = module_hmi_handle,
    .config = module_hmi_config,
    .tamperwarn = module_hmi_tamper_warn_time,
};

// ------------------------------------------

const bsp_hmi_drive_io_t* bsp_hmi_get_driver(bsp_hmi_callback_t callback)
{
    hmiEventRegister_callback(callback);
    return &bsp_hmi;
}
