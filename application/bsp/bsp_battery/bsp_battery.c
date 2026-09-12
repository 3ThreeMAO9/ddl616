#include "bsp_battery.h"

/********************Variable********************/
const bsp_battery_drive_io_t bsp_battery = {
    .init = module_batteryInit,
    .loop = module_batteryLoop,
    .scan = module_batteryScan,
    .voltage = module_batteryReadVoltage,
    .percent = module_batteryReadPercent,
    .state = module_batteryReadState,

};

// ----------------------------------------

const bsp_battery_drive_io_t* bsp_battery_get_driver(bsp_battery_callback_t callback)
{
    batteryEventRegister_callback(callback);

    return &bsp_battery;
}
