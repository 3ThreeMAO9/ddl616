#include "motor.h"

#define OB_LOG_LEVEL OB_LOG_LEVEL_NONE
#include "ob_log.h"
#define TAG "motor"

/***************Variable***************/
void motorDriver(uint8_t motorStep, uint8_t direction)
{
#if (Enabled == PRINTF_MOTOR)
    OB_LOGD(TAG, "motorDriver motorStep[%d]", motorStep);
#endif
    switch (motorStep)
    {
        case MOTOR_STEP_UNLOCK:
            SET_MOTOR_LA_HIGH;
            CLR_MOTOR_LB_LOW;
            break;
        case MOTOR_STEP_UNLOCK_REVERSE:
            CLR_MOTOR_LA_LOW;
            SET_MOTOR_LB_HIGH;
            break;
        case MOTOR_STEP_BRAKE:
        case MOTOR_STEP_SLEEP:
        case MOTOR_STEP_UNLOCK_BREAK:
        case MOTOR_STEP_IDLE:
            CLR_MOTOR_LA_LOW;
            CLR_MOTOR_LB_LOW;
            break;
        default:
            MOTOR_LA_SLEEP();
            MOTOR_LB_SLEEP();
            break;
    }
}
