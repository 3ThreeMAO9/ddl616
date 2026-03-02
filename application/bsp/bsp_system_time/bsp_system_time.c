#include "bsp_system_time.h"


/***************Variable***************/
const bsp_system_time_drive_io_t bsp_system_time = {
    .init = system_timer_init,
    .loop = system_timer_loop,
    .time_out = set_system_timer_out,
    .sleep = system_timer_sleep,

    .timer_time_out = system_out_time_cnt,
    .timer_inc_cnt = system_inc_time_cnt,
    .ms_tick = system_ms_get,

};

// ------------------------------------------

const bsp_system_time_drive_io_t* bsp_system_time_get_driver(bsp_system_time_callback_t callback)
{
    system_timer_event_register_callback(callback);
    return &bsp_system_time;
}
