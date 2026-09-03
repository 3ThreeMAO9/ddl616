#include "bsp_motor.h"

/***************Variable***************/
const bsp_motor_drive_io_t bsp_motor = {
    .init = module_motorInit,
    .loop = module_motorLoop,
    .handle = module_motorHandle,
    .motorBusy = module_motorReadMotorBusyAndWarn,
};

// ------------------------------------------

const bsp_motor_drive_io_t* bsp_motor_get_driver(bsp_motor_callback_t callback)
{
    motorEventRegister_callback(callback);
    return &bsp_motor;
}
