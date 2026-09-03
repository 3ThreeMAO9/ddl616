#include "module_motor.h"
#include "motor.h"
#include "event.h"
#include "hal_timer.h"
#include "hal_wdt.h"
#include "system_timer.h"
#include "parameter.h"
// #include "module_produce_test.h"
// #include "shell.h"
#include "led.h"
#include "task_hmi.h"
#include "task_motor.h"
#include "task_player.h"
// #include "task_battery.h"
// #include "lock_log.h"

#define OB_LOG_LEVEL OB_LOG_LEVEL_DEBUG
#include "ob_log.h"
#define TAG "m_motor"

/***************Variable***************/
static motor_handle_t motorHandle;
static motor_callback_t motor_callback;
static uint8_t aging_mode = 0;

// ------------------------------------------
uint8_t isagingmode(void)
{
    return aging_mode;
}

void set_agingmode(uint8_t mode)
{
    aging_mode = mode;
}

static void motorHandleEvent_callback(uint8_t event, uint32_t value)
{
    if (NULL != motor_callback)
    {
        switch (motorHandle.handle)
        {
        case MOTOR_HANDLE_UNLOCK:
            break;
        default:
            break;
        }
        motor_callback(event, value);
    }
#if (Enabled == PRINTF_ERR)
    else
    {
        OB_LOGE(TAG, "Err: motor callback is null");
    }
#endif
}

static void motorGpioInit(void)
{
    motorDriver(MOTOR_STEP_INIT, 0);
}

void module_motorInit(void)
{
#if (Enabled == PRINTF_MOTOR)
    OB_LOGD(TAG, "module motor init");
#endif
    motorGpioInit();

    memset((uint8_t *)(&motorHandle), 0, sizeof(motor_handle_t));
    motorHandle.scanTimeOut = system_inc_time_cnt(0);
}

//  @brief 注册回调函数
//  @param callback
void motorEventRegister_callback(motor_callback_t callback)
{
    motor_callback = callback;
}

static void motorHandleStartup(uint8_t step, uint8_t duty, uint32_t timeOut)
{
#if (Enabled == PRINTF_MOTOR)
    OB_LOGD(TAG, "motor startup: step[%u], duty[%u], timeOut[%u]", step, duty, timeOut);
#endif
    motorHandle.motorStep = step;
    motorHandle.duty = duty;
    motorHandle.timeOut = system_inc_time_cnt(timeOut);

    motorDriver(motorHandle.motorStep, 0);
}

void module_motorHandle(uint8_t handle, uint16_t value)
{
    if (motorHandle.busy)
    {
#if (Enabled == PRINTF_MOTOR)
        OB_LOGE(TAG, "Fail[handle]: motor is busy");
#endif
        return;
    }

    motorHandle.handle = handle;

    switch (handle)
    {
    case MOTOR_HANDLE_UNLOCK:
        OB_LOGD(TAG, "motor unlock");
        motorHandle.busy = true;
        motorHandleStartup(MOTOR_STEP_UNLOCK, 0, MOTOR_UNLOCK_TIME_OUT);
        break;
    default:
        break;
    }
}

static void motorUnlockLoop(void)
{
    switch (motorHandle.motorStep)
    {
    case MOTOR_STEP_UNLOCK:
        if (system_out_time_cnt(motorHandle.timeOut))
        {
#if (Enabled == PRINTF_MOTOR)
            OB_LOGI(TAG, "%s ===TP[%u]: value[%lu]=== MOTOR_STEP_UNLOCK", __FUNCTION__, motorHandle.motorStep, 0);
#endif
            motorHandleStartup(MOTOR_STEP_UNLOCK_BREAK, 0, MOTOR_UNLOCK_BRAKE_TIME_OUT);
            motorHandleEvent_callback(MOTOR_EVENT_UNLOCKED, MOTOR_TYPE_NULL);
        }
        break;

    case MOTOR_STEP_UNLOCK_BREAK:
        if (system_out_time_cnt(motorHandle.timeOut))
        {
#if (Enabled == PRINTF_MOTOR)
            OB_LOGI(TAG, "%s ===TP[%u]: value[%lu]=== MOTOR_STEP_UNLOCK_BREAK", __FUNCTION__, motorHandle.motorStep, 0);
#endif
            motorHandleStartup(MOTOR_STEP_UNLOCK_REVERSE, 0, MOTOR_UNLOCK_REVERSE_TIME_OUT);
        }
        break;
    case MOTOR_STEP_UNLOCK_REVERSE:
        if (system_out_time_cnt(motorHandle.timeOut))
        {
#if (Enabled == PRINTF_MOTOR)
            OB_LOGI(TAG, "%s ===TP[%u]: value[%lu]=== MOTOR_STEP_UNLOCK_REVERSE", __FUNCTION__, motorHandle.motorStep, 0);
#endif
            motorHandleStartup(MOTOR_STEP_BRAKE, 0, MOTOR_BRAKE_TIME_OUT);
        }
        break;
    case MOTOR_STEP_BRAKE:
        if (system_out_time_cnt(motorHandle.timeOut))
        {
#if (Enabled == PRINTF_MOTOR)
            OB_LOGI(TAG, "%s ===TP[%u]: value[%lu]=== MOTOR_STEP_BRAKE", __FUNCTION__, motorHandle.motorStep, 0);
#endif
            motorHandleStartup(MOTOR_STEP_IDLE, 0, 0);
            motorHandle.handle = MOTOR_HANDLE_NULL;
            motorHandle.busy = false;
            motorHandleEvent_callback(MOTOR_EVENT_LOCKED, MOTOR_TYPE_NULL);
        }
        break;
    default:
        break;
    }
}

uint8_t module_motorReadMotorBusyAndWarn(void)
{
    return (motorHandle.busy | motorHandle.warnBusy);
}

void module_motorLoop(void)
{
    switch (motorHandle.handle)
    {
    case MOTOR_HANDLE_UNLOCK:
        motorUnlockLoop();
        break;
    default:
        break;
    }
}
