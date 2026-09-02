#include "module_hmi.h"
#include "hal_timer.h"
#include "utils.h"
#include "led.h"
#include "parameter.h"
#include "bsp_voice_list.h"
#include "task_player.h"
// #include "task_battery.h"
// #include "task_motor.h"
#include "task_sleep.h"
#include "task_key.h"
#include "hal_rtc.h"
#include "timestamp.h"
#include "system_timer.h"

#define OB_LOG_LEVEL OB_LOG_LEVEL_DEBUG
#include "ob_log.h"
#define TAG "m_hmi"
/***************Variable***************/
static hmi_handle_t hmihandle;

/*****************Macro****************/
#define HMI_HANDLE_INIT()           memset((uint8_t*)(&hmihandle), 0, sizeof(hmi_handle_t))

// ------------------------------------------

void module_hmi_init(void)
{
#if (Enabled==PRINTF_HMI)
    OB_LOGD(TAG,"Init: HMI");
#endif
    HMI_HANDLE_INIT();
    ledInit();
}

static void hmi_logo_led_config(uint8_t nowColor, uint8_t nextColor, uint32_t halfPeriod, uint32_t halfPeriodCnt)
{
    hmihandle.logoLed.color = BCD_CONVERT_HEX(nextColor, nowColor);
    hmihandle.logoLed.halfPeriodCnt = halfPeriodCnt;
    hmihandle.logoLed.timeOut = system_inc_time_cnt(halfPeriod);

    if (halfPeriodCnt)
    {
        hmihandle.busy |= HMI_BUSY_BIT_LOGO_LED;
        hmihandle.logoLed.halfPeriod = halfPeriod;
    }

    logoLedDrive(hmihandle.logoLed.color);
}


static void hmi_key_board_led_config(uint8_t nowState, uint8_t nextState, uint32_t halfPeriod, uint32_t halfPeriodCnt)
{
    hmihandle.keyBoardLed.state = BCD_CONVERT_HEX(nextState, nowState);
    hmihandle.keyBoardLed.halfPeriodCnt = halfPeriodCnt;
    hmihandle.keyBoardLed.timeOut = system_inc_time_cnt(halfPeriod);

    if (halfPeriodCnt)
    {
        hmihandle.busy |= HMI_BUSY_BIT_KEY_BOARD_LED;
        hmihandle.keyBoardLed.halfPeriod = halfPeriod;
    }

    keyBoardLedDrive(hmihandle.keyBoardLed.state);
}


uint32_t module_hmi_handle(uint8_t state, uint8_t silentFlag)
{
//    struct tm *time_info;
    uint32_t keepTime = HANDLE_RESULT_TIME_OUT;

    switch (state)
    {
        case HMI_STATE_NULL_IDLE:
            keepTime = 0;
            break;
        case HMI_STATE_KEY_BOARD_PRESS:
            if (!silentFlag)
            {
                PLAYER_LIST_CLEAR_ADD(SOUND_BUTTON_DI);
            }
            keepTime = HMI_STATE_KEEP_TIME_90ms;
            break;
        case HMI_STATE_TIME_OUT:
            keepTime = HMI_STATE_KEEP_TIME_100ms;
            break;
        case HMI_STATE_POWER_ON:
            hmi_logo_led_config(LOGO_LED_COLOR_BLUE, LOGO_LED_COLOR_IDLE, HMI_STATE_KEEP_TIME_1s, 1);
            hmi_key_board_led_config(TRUN_ON, TRUN_ON, 0, 0);
            keepTime = HMI_STATE_KEEP_TIME_1s;
            break;
        case HMI_STATE_KEY_BOARD_LED_ON:
            hmi_key_board_led_config(TRUN_ON, TRUN_ON, 0, 0);
            break;
        case HMI_STATE_KEY_BOARD_LED_OFF:
            hmi_key_board_led_config(TRUN_OFF, TRUN_OFF, 0, 0);
            break;
        case HMI_STATE_KEY_BOARD_LED_BLUE_OB:
            hmi_logo_led_config(LOGO_LED_COLOR_BLUE, LOGO_LED_COLOR_IDLE, HMI_STATE_KEEP_TIME_1s, 1);
            hmi_key_board_led_config(TRUN_ON, TRUN_ON, 0, 0);
            break;
        default:
            return keepTime;
    }

    hmihandle.state = state;
    keepTime += 100;

#if (Enabled==PRINTF_HMI)
    OB_LOGD(TAG,"logo led[%u] start", state);
#endif	
	return keepTime;
}

void module_hmi_config(uint8_t sleepFlag)
{
    if (sleepFlag)
    {
        HMI_HANDLE_INIT();
        keyBoardLedDrive(hmihandle.keyBoardLed.state);
        logoLedDrive(hmihandle.logoLed.color);
    }
}

static void hmi_logo_led_loop(void)
{
    hmihandle.logoLed.color = SWAP_HIGH_LOW_4BITS(hmihandle.logoLed.color);
    if (hmihandle.logoLed.halfPeriodCnt)
    {
        hmihandle.logoLed.halfPeriodCnt--;
        
        if (0 == hmihandle.logoLed.halfPeriodCnt)
        {
            switch (hmihandle.state)
            {
                case HMI_STATE_VERIFY_SUCCESS:
                case HMI_STATE_VERIFY_ADMIN_SUCCESS:
                case HMI_STATE_HANDLE_SUCCESS_KEEP_GREEN:
                case HMI_STATE_ENROLL_PRESS_TWICE:
                    hmihandle.logoLed.color = LOGO_LED_COLOR_GREEN;
                    break;
                case HMI_STATE_HANDLE_FAIL_KEEP_RED:
                    hmihandle.logoLed.color = LOGO_LED_COLOR_RED;
                    break;
                default:
                    hmihandle.logoLed.color = LOGO_LED_COLOR_IDLE;
                    break;
            }
            OB_LOGW(TAG,"hmihandle.state [%ld] hmihandle.logoLed.color[%ld]",hmihandle.state,hmihandle.logoLed.color);
            hmihandle.logoLed.halfPeriod = 0;
            hmihandle.busy &= (~HMI_BUSY_BIT_LOGO_LED);
        }
    }
    logoLedDrive(hmihandle.logoLed.color);
}

static void hmi_key_board_led_loop(void)
{
    hmihandle.keyBoardLed.state = SWAP_HIGH_LOW_4BITS(hmihandle.keyBoardLed.state);

    if (hmihandle.keyBoardLed.halfPeriodCnt)
    {
        hmihandle.keyBoardLed.halfPeriodCnt--;
        if (0 == hmihandle.keyBoardLed.halfPeriodCnt)
        {
            hmihandle.keyBoardLed.halfPeriod = 0;
            hmihandle.busy &= (~HMI_BUSY_BIT_KEY_BOARD_LED);
            switch (hmihandle.state)
            {
                case HMI_STATE_VACATION_MODE_WARN:
                case HMI_STATE_VACATION_MODE_FAIL_WARN:
                    hmihandle.keyBoardLed.state = TRUN_OFF;
                    break;
                case HMI_STATE_NULL_IDLE:
                    hmihandle.keyBoardLed.state = TRUN_OFF;
                    return;
                default:
                    hmihandle.keyBoardLed.state = TRUN_ON;
                    break;
            }
        }
    }
    keyBoardLedDrive(hmihandle.keyBoardLed.state);
}

void module_hmi_loop(void)
{
    if (hmihandle.keyBoardLed.halfPeriod && system_out_time_cnt(hmihandle.keyBoardLed.timeOut))
    {
        hmihandle.keyBoardLed.timeOut = system_inc_time_cnt(hmihandle.keyBoardLed.halfPeriod);

        hmi_key_board_led_loop();
    }

    if (hmihandle.logoLed.halfPeriod && system_out_time_cnt(hmihandle.logoLed.timeOut))
    {
        hmihandle.logoLed.timeOut = system_inc_time_cnt(hmihandle.logoLed.halfPeriod);

        hmi_logo_led_loop();
    }
}
