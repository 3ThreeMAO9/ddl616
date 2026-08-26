#include "task_system_time.h"
#include "hal_rtc.h"
#include "event.h"


#define OB_LOG_LEVEL OB_LOG_LEVEL_DEBUG
#include "ob_log.h"
#define TAG "task_system_time"

/***************Variable***************/
static system_time_task_driver_t system_time_task_driver;

// ------------------------------------------

static uint16_t system_time_task_callback(uint8_t source, uint8_t timeOutId)
{
    // OB_LOGD(TAG,"%s source[%ld] timeOutId[%ld]",__func__,source,timeOutId);
    if (DOWN_COUNT_SOURCE_TIMER == source){
        switch (timeOutId){
            case SYSTEM_TIME_OUT_ID:
                baseEventPush(Q_WORK_TIME_OUT_SIG, 0);
                break;
            case MODULE_TIME_OUT_ID:
                baseEventPush(Q_FUNCTION_TIME_OUT_SIG, 0);
                break;
            default:
                break;
        }
    }
    else if (DOWN_COUNT_SOURCE_WDT == source) {

    }
    else if (DOWN_COUNT_SOURCE_RTC == source) {
        hal_get_rtc_time();
    }
    return true;
}

void system_time_task_set_work_time(uint32_t downCount)
{
    system_time_task_driver.io->time_out(DOWN_COUNT_SOURCE_TIMER, SYSTEM_TIME_OUT_ID, downCount);
}

void system_time_task_set_function_time(uint32_t downCount)
{
    system_time_task_driver.io->time_out(DOWN_COUNT_SOURCE_TIMER, MODULE_TIME_OUT_ID, downCount);
}


void system_time_task_set_timer_timeout(uint8_t id, uint32_t downCount)
{
    system_time_task_driver.io->time_out(DOWN_COUNT_SOURCE_TIMER, id, downCount);
}

void system_time_task_init(void)
{
    OB_LOGD(TAG, "Init[%s]", __func__);
    system_time_task_driver.io = bsp_system_time_get_driver(system_time_task_callback);
    if (NULL != system_time_task_driver.io){
        system_time_task_driver.io->init(DOWN_COUNT_SOURCE_TIMER);
        system_time_task_driver.io->init(DOWN_COUNT_SOURCE_WDT);
        system_time_task_driver.io->init(DOWN_COUNT_SOURCE_RTC);
    }
    else{
        OB_LOGE(TAG, "Error[%s]", "system_time_task_driver.io is NULL");
    }
}

uint16_t system_time_task_loop(void) 
{
    return system_time_task_driver.io->loop();
}

void system_time_task_sleep(uint8_t event)
{
    system_time_task_driver.io->sleep(event);
}
