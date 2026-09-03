#include "task_motor.h"
// #include "task_produce_test.h"
#include "task_hmi.h"
// #include "task_battery.h"
#include "system_timer.h"
#include "event.h"
#include "parameter.h"
// #include "msg_protocol_kds_017.h"
// #include "lock_log.h"

// #include "shell.h"
#include "task_player.h"
#define OB_LOG_LEVEL OB_LOG_LEVEL_DEBUG
#include "ob_log.h"
#define TAG "task_motor"
/***************Variable***************/
static motor_task_driver_t motor_task_driver;

// ------------------------------------------

static void motorTask_callback(uint8_t event, uint32_t value)
{
    OB_LOGW(TAG, "motor task callback[event:%u][value:%u]", event, value);

    switch (event)
    {
    case MOTOR_EVENT_VACATION_MODE_WARN:
        break;
    case MOTOR_EVENT_UNLOCKED:
        break;
    case MOTOR_EVENT_LOCKED:
        baseEventPush(Q_HANDLE_SIG, EVENT_RESULT_SUCCESS_LOCKED);
        break;
    case MOTOR_EVENT_IDLE:
        break;
    default:
        break;
    }
}

void motorTaskHandle(uint8_t handleCmd, uint8_t value)
{
    motor_task_driver.io->handle(handleCmd, value);
#if (Enabled == PRINTF_MOTOR)
    OB_LOGD(TAG, "motorTask handle[%u], value[%u]", handleCmd, value);
#endif
}


void motorTaskInit(void)
{
#if (Enabled == PRINTF_MOTOR)
    OB_LOGD(TAG, "Init: motor");
#endif
    motor_task_driver.io = bsp_motor_get_driver(motorTask_callback);
    if (NULL != motor_task_driver.io)
    {
        motor_task_driver.io->init();
    }
#if (Enabled == PRINTF_ERR)
    else
    {
        OB_LOGE(TAG, "Err: motor_task_driver.io is null");
    }
#endif
}

void motorTaskLoop(void)
{
    motor_task_driver.io->loop();
}


