#include "module_produce_test.h"
#include "PCBA_test.h"
#include "module_hmi.h"
#include "system_timer.h"
#include "parameter.h"
#include "event.h"
#include "user.h"
#include "task_battery.h"
#include "hal_rtc.h"
#include "produce_test.h"
#include "hal_gpio.h"
#include "config.h"
#include "hal_wdt.h"

#define OB_LOG_LEVEL OB_LOG_LEVEL_DEBUG
#include "ob_log.h"
#define TAG "module_produce_test"

/***************Variable***************/
static produce_test_handle_t produceTestHandle;
static produce_test_callback_t produceTest_callback;
static void produceTestHandleEvent_callback(uint8_t result, uint8_t value)
{
    if (NULL != produceTest_callback)
    {
        produceTest_callback(result, value);
    }
#if (Enabled == PRINTF_ERR)
    else
    {
        OB_LOGE(TAG, "Err: produce test' callback is null");
    }
#endif
}
void produceTestEventRegister_callback(produce_test_callback_t callback)
{
    produceTest_callback = callback;

#if (Enabled == PRINTF_TEST)
    OB_LOGD(TAG, "produce_test_register_callback:%X", callback);
#endif
}

void produceTestInit(void)
{
    const uint8_t keyBoardTab[KEY_CNT] = DEVICE_TEST_KEY_BOARD_TAB;
    memset((uint8_t *)(&produceTestHandle), 0, sizeof(produce_test_handle_t));
    memcpy((uint8_t *)(produceTestHandle.keyBoard.tab), keyBoardTab, KEY_CNT);
    produceInfoInit();
    // if (isEmptyUser(false))
    //     dev_init_resp(); // 没用户，默认上电会发送数据给产测工具
    produceTestHandleEvent_callback(EVENT_RESULT_PRODUCE_INIT, 0);
}


void produceTestLoop(void)
{

}
