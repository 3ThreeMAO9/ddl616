#include "module_battery.h"
#include "hal_adc.h"
#include "hal_timer.h"
#include "system_timer.h"
#include "parameter.h"

#define OB_LOG_LEVEL OB_LOG_LEVEL_DEBUG
#include "ob_log.h"
#define TAG "m_battery"

/********************Variable********************/
static battery_handle_t batteryHandle;
static battery_callback_t battery_callback;

/********************Macro********************/
#define CLEAR_BATTERY_HANDLE() memset((uint8_t *)(&batteryHandle), 0, sizeof(battery_handle_t));

// ----------------------------------------

static uint8_t calculateBatteryPercent(uint16_t voltage)
{
    uint8_t percent;

    if (voltage <= BATTERY_VOL_PERCENT_0)
    {
        percent = 0;
    }
    else if (voltage >= BATTERY_VOL_PERCENT_100)
    {
        percent = 100;
    }
    else
    {
        percent = ((voltage - BATTERY_VOL_PERCENT_0) * 100 / (BATTERY_VOL_PERCENT_100 - BATTERY_VOL_PERCENT_0));
    }

    return percent;
}

static void batteryHandleEvent_callback(uint8_t event)
{
    if (NULL != battery_callback)
    {
        battery_callback(event);
    }
#if (Enabled == PRINTF_ERR)
    else
    {
        OB_LOGE(TAG, "Err: battery' callback is NULL");
    }
#endif
}

static uint16_t scanBatteryVoltage(uint16_t compensationValue)
{
    uint16_t voltage;

    voltage = (hal_readChannelCurrentCalibrate(BATTERY_ADC_CHANNEL, BATTERY_VOL_SAMPLE_CNT) * 3);
    voltage += compensationValue; // 电压补偿值(二极管压降)

#if (Enabled == PRINTF_VOLTAGE)
    OB_LOGD(TAG, "battery voltage - real[%u  %u]", voltage, voltage - compensationValue);
#endif
    return voltage;
}

static void updateBatteryParameter(uint16_t voltage)
{
    if ((voltage > batteryHandle.voltage) && ((voltage - batteryHandle.voltage) < BATTERY_VOL_RAISE_UPDATE_VALUE))
    {
        return; // 单调递减回升限制条件
    }
    // if (voltage > batteryHandle.voltage)
    // {
    //     return;     // 单调递减限制条件
    // }

    batteryHandle.voltage = voltage;
    batteryHandle.percent = calculateBatteryPercent(voltage);

    if (batteryHandle.voltage < BATTERY_VOL_LOW_SYSTEM_LOCK_TH)
    {
        batteryHandle.lowState = BATTERY_STATE_LOW_SYSTEM_LOCK;
    }
    else if (batteryHandle.voltage < BATTERY_VOL_LOW_WARN_TH)
    {
        batteryHandle.lowState = BATTERY_STATE_LOW_WARN;
    }
    else
    {
        batteryHandle.lowState = BATTERY_STATE_NORMAL;
    }
}

void module_batteryScan(uint16_t compensationValue)
{
    uint16_t voltage;

    voltage = scanBatteryVoltage(compensationValue);
    updateBatteryParameter(voltage);

#if (Enabled == PRINTF_VOLTAGE)
    OB_LOGD(TAG, "battery voltage - result[vol: %u, percent: %u%%, low: %u]", batteryHandle.voltage, batteryHandle.percent, batteryHandle.lowState);
#endif
}

uint16_t module_batteryReadVoltage(void)
{
    // OB_LOGD(TAG,"battery voltage - result[vol: %u   %04X]", batteryHandle.voltage, batteryHandle.voltage);
    return batteryHandle.voltage;
}

uint8_t module_batteryReadPercent(void)
{
    return batteryHandle.percent;
}

uint8_t module_batteryReadState(void)
{
    return batteryHandle.lowState;
}

void module_batteryInit(void)
{
    uint16_t bat_vol;

#if (Enabled == PRINTF_VOLTAGE)
    OB_LOGD(TAG, "Init: module battery");
#endif

    BATTERY_VOLTAGE_PIN_INIT();
    bat_vol = scanBatteryVoltage(BATTERY_VOL_COMPENSATION_VALUE);
    batteryHandle.lowState = BATTERY_STATE_NORMAL;
    updateBatteryParameter(bat_vol);
#if (Enabled == PRINTF_VOLTAGE)
    OB_LOGD(TAG, "batteryHandle.lowState=%d", batteryHandle.lowState);
#endif
}

void module_batteryLoop(void)
{
    static uint8_t stateLast = BATTERY_STATE_NORMAL;

    switch (batteryHandle.lowState)
    {
    case BATTERY_STATE_LOW_WARN:
        if (stateLast != batteryHandle.lowState)
        {
            batteryHandle.timeOut = system_inc_time_cnt(0);
        }
        else if (system_out_time_cnt(batteryHandle.timeOut))
        {
            batteryHandle.timeOut = system_inc_time_cnt(BATTERY_LOW_WARN_TIME);
            batteryHandleEvent_callback(BATTERY_EVENT_STATE_LOW_WARN);
        }
        break;
    case BATTERY_STATE_NORMAL:
        break;
    case BATTERY_STATE_LOW_SYSTEM_LOCK:
        break;
    default:
        break;
    }

    stateLast = batteryHandle.lowState;
}

// @brief 注册回调函数
// @param callback
void batteryEventRegister_callback(battery_callback_t callback)
{
    battery_callback = callback;

#if (Enabled == PRINTF_VOLTAGE)
    OB_LOGD(TAG, "batteryEventRegister_callback:%X", callback);
#endif
}

uint16_t TestBatteryVoltage(void)
{
    uint16_t voltage;

    voltage = 0;
#if (Enabled == PRINTF_VOLTAGE)
    OB_LOGD(TAG, "battery voltage - real[%u]", voltage);
#endif
    // uart0_printf("%d\r\n",voltage);
    return voltage;
}
