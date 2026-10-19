#include "task_motor.h"
// #include "task_produce_test.h"
#include "task_hmi.h"
// #include "task_battery.h"
#include "system_timer.h"
#include "event.h"
#include "parameter.h"

// #include "lock_log.h"

// #include "shell.h"
#include "task_player.h"
#define OB_LOG_LEVEL OB_LOG_LEVEL_NONE
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
    case MOTOR_EVENT_LOCKED:
        baseEventPush(Q_HANDLE_SIG, EVENT_RESULT_SUCCESS_LOCKED);
        break;
    // case MOTOR_EVENT_OPENED:
    //     baseEventPush(Q_HANDLE_SIG, EVENT_RESULT_SUCCESS_OPENED);
    //     break;
    // case MOTOR_EVENT_CLOSED:
    //     baseEventPush(Q_HANDLE_SIG, EVENT_RESULT_SUCCESS_CLOSED);
    //     break;
    default:
        break;
    }
}

void motorTaskHandle(uint8_t handleCmd, uint8_t value)
{
    // 通道模式：一直保持开锁状态
    if (Enabled == readUserParameter(USER_PARA_CHANNEL_MODE_ID))
    {
        if (MOTOR_HANDLE_CLOSE == handleCmd)
        {
            return;                                     // 禁止上锁
        }
        if (MOTOR_HANDLE_UNLOCK == handleCmd)
        {
            handleCmd = MOTOR_HANDLE_OPEN;              // UNLOCK 是"开锁→延时→关锁"，改走只开不关
        }
    }

    motor_task_driver.io->handle(handleCmd, value);
#if (Enabled == PRINTF_MOTOR)
    OB_LOGD(TAG, "motorTask handle[%u], value[%u]", handleCmd, value);
#endif
}

// 门锁状态：1=关锁 2=开锁
uint8_t motorTaskReadDoorState(void)
{
    return module_motorReadDoorState();
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


