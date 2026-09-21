#include "bsp_produce_test.h"


/***************Variable***************/
const bsp_produce_test_drive_io_t bsp_produce_test = {
    .init = produceTestInit,
    // .uartConfig = produceTestHwConfig,
    .loop = produceTestLoop,
    // .keyBoard = pcbaKeyBoardTestPressHandle,
    // .motor = pcbaMotorTestHandle,
    // .nfc = pcbaNfcTestPressHandle,
    // .key = pcbaKeyTestHandle,
};

// ------------------------------------------

const bsp_produce_test_drive_io_t* bsp_produce_test_get_driver(bsp_produce_test_callback_t callback)
{
    produceTestEventRegister_callback(callback);
    return &bsp_produce_test;
}
