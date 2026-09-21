#include "task_produce_test.h"
#include "event.h"
#include "system_timer.h"
#include "task_hmi.h"
#include "task_key.h"
#define OB_LOG_LEVEL OB_LOG_LEVEL_DEBUG
#include "ob_log.h"
#define TAG "task_produce_test"

/***************Variable***************/
static produce_test_task_driver_t produce_test_task_driver;

// ------------------------------------------

static void produceTestTask_callback(uint8_t event, uint8_t value)
{
#if (Enabled == PRINTF_TEST)
    OB_LOGD(TAG, "produce test: callback[%u, %u]", event, value);
#endif
    // if (EVENT_RESULT_PRODUCE_TEST == event)
    // {
    //     produceTestTaskSetType(PRODUCE_TEST_PCBA);
    // }
    handleEventPush(event, value);
}

// uint8_t produceTestTaskGetType(void)
// {
//     return produce_test_task_driver.attribute.type;
// }

// void produceTestTaskSetType(uint8_t testType)
// {
//     produce_test_task_driver.attribute.type = testType;
// }

// void produceTestTaskSetStep(uint8_t step)
// {
// #if (Enabled == PRINTF_TEST)
//     OB_LOGD(TAG, "Produce step[%u]", step);
// #endif
//     produce_test_task_driver.attribute.step = step;

//     if (DEVICE_TEST_STEP_IDLE == produce_test_task_driver.attribute.step)
//     {
//         produce_test_task_driver.io->init();
//     }
// }

// uint8_t produceTestTaskGetStep(void)
// {
//     return produce_test_task_driver.attribute.step;
// }

// void produceTestTaskUartConfig(uint8_t initFlag)
// {
//     produce_test_task_driver.io->uartConfig(initFlag);
// }

// uint8_t produceTestTaskKeyBoard(uint8_t keyValue)
// {
//     uint8_t result;

//     if (PRODUCE_TEST_IDLE != produce_test_task_driver.attribute.type)
//     {
//         result = produce_test_task_driver.io->keyBoard(keyValue);
//         if (result)
//         {
//             if (true == result)
//             {
//                 keyTaskHandle(KEY_TYPE_KEY_BOARD, false); // key board
//             }
//         }
//     }
//     else
//     {
//         result = false;
//     }

//     return result;
// }

// void produceTestTaskMotor(uint32_t value)
// {
//     produce_test_task_driver.io->motor(value);
// }

// void produceTestTaskNfc(uint8_t flag)
// {
//     produce_test_task_driver.io->nfc(flag);
// }

// void produceTestTaskKey(uint8_t value)
// {
//     produce_test_task_driver.io->key(value);
// }

void produceTestTaskInit(void)
{
#if (Enabled == PRINTF_TEST)
    OB_LOGD(TAG, "Init: produce test");
#endif
    produce_test_task_driver.io = bsp_produce_test_get_driver(produceTestTask_callback);
    if (NULL != produce_test_task_driver.io)
    {
        // produceTestTaskUartConfig(true);
        produce_test_task_driver.io->init();
        produce_test_task_driver.attribute.type = PRODUCE_TEST_IDLE;
        produce_test_task_driver.attribute.step = DEVICE_TEST_STEP_IDLE;
    }
}

void produceTestTaskLoop(void)
{
    produce_test_task_driver.io->loop();
}
