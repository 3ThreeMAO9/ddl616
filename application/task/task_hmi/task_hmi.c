#include "task_hmi.h"
#include "parameter.h"
#include "event.h"

#define OB_LOG_LEVEL OB_LOG_LEVEL_NONE
#include "ob_log.h"
#define TAG "task_hmi"

/***************Variable***************/
static hmi_task_driver_t hmi_task_driver;
static uint8_t isallowSlientFlag=true;

// ------------------------------------------
static void hmiTask_callback(uint8_t event, uint32_t value)
{
    OB_LOGW(TAG, "hmi task callback[event:%u][value:%u]", event, value);
    if (event == HMI_STATE_TAMPER_WARN)
    {
        handleEventPush(EVENT_RESULT_BREAK_WARN, value);
    }
}
void hmiTaskInit(void)
{
#if (Enabled == PRINTF_HMI)
    OB_LOGD(TAG,"Init: hmi init");
#endif
    hmi_task_driver.io = bsp_hmi_get_driver(hmiTask_callback);
    if (NULL != hmi_task_driver.io)
    {
        hmi_task_driver.io->init();
    }
#if (Enabled == PRINTF_ERR)
    else
    {
        OB_LOGE(TAG,"Err: hmi_task_driver.io is null");
    }
#endif
}

void hmiTaskSetAllowSelintFlag(uint8_t allowSelintFlag)
{
    isallowSlientFlag=allowSelintFlag;
    if (allowSelintFlag && (!readUserParameter(USER_PARA_SILENT_MODE_ID)))
    {
        hmi_task_driver.attribute.silentFlag = Enabled;
    }
    else
    {
        hmi_task_driver.attribute.silentFlag = Disabled;
    }
#if (Enabled == PRINTF_HMI)
    OB_LOGD(TAG,"hmi set silent[%u]", hmi_task_driver.attribute.silentFlag);
#endif
}

uint32_t hmiTaskSetState(uint8_t state)
{
#if (Enabled == PRINTF_HMI)
    OB_LOGD(TAG,"hmi set state[%u], silent[%u]", state, hmi_task_driver.attribute.silentFlag);
#endif

    return hmi_task_driver.io->setState(state, hmi_task_driver.attribute.silentFlag);
}

void hmiTaskConfig(uint8_t sleepFlag)
{
    hmi_task_driver.io->config(sleepFlag);
}

void hmiTaskLoop(void)
{
    hmi_task_driver.io->loop();
}

uint8_t isallowSlient(void)
{
    return isallowSlientFlag;
}

void hmi_task_tamper_warn_time(uint32_t time)
{
    if (time != 0)
        setUserParameter(USER_PARA_BREAK_ID, Enabled);
    else
        setUserParameter(USER_PARA_BREAK_ID, Disabled);
    hmi_task_driver.io->tamperwarn(time);
}

uint8_t hmi_task_tamper_warn_is_busy(void)
{
    return hmi_task_driver.io->tamperwarn_busy();
}

