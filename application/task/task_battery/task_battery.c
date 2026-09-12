#include "task_battery.h"
#include "task_hmi.h"

#define OB_LOG_LEVEL OB_LOG_LEVEL_NONE
#include "ob_log.h"
#define TAG "task_battery"

/********************Variable********************/
static battery_task_driver_t battery_task_driver;

// ----------------------------------------

static void batteryTask_callback(uint8_t event)
{
    if (battery_task_driver.attribute.enable)
    {
#if (Enabled == PRINTF_VOLTAGE)
        OB_LOGD(TAG,"battery task callback[event:%u]", event);
#endif
        hmiTaskSetState(HMI_STATE_LOW_ENERGY);
    }
}

void batteryTaskInit(void)
{
#if (Enabled == PRINTF_HMI)
    OB_LOGD(TAG,"Init: module_battery task");
#endif
    battery_task_driver.io = bsp_battery_get_driver(batteryTask_callback);
    if (NULL != battery_task_driver.io)
    {
        battery_task_driver.io->init();
        batteryTaskWarnConfig(false);
    }
#if (Enabled == PRINTF_ERR)
    else
    {
        OB_LOGE(TAG,"Err: battery_task_driver.io is null");
    }
#endif
}

void batteryTaskWarnConfig(uint8_t flag)
{
    battery_task_driver.attribute.enable = flag;
}

uint8_t batteryTaskReadState(void)
{
    return battery_task_driver.io->state();
}

uint16_t batteryTaskReadVoltage(void)
{
    return battery_task_driver.io->voltage();
}

uint8_t batteryTaskReadPercent(void)
{
    return battery_task_driver.io->percent();
}

bool isBatteryLow(void)
{
    return (batteryTaskReadState() != BATTERY_STATE_NORMAL) ? true : false;
}

void batteryTaskScan(uint16_t compensationValue)
{
    battery_task_driver.io->scan(compensationValue);
}

void batteryTaskLoop(void)
{
    battery_task_driver.io->loop();
}

void get_battery_test(void)
{
    batteryTaskScan(BATTERY_VOL_COMPENSATION_VALUE);
}
