#include "module_hmi.h"
#include "hal_timer.h"
#include "utils.h"
#include "user.h"
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

#define OB_LOG_LEVEL OB_LOG_LEVEL_NONE
#include "ob_log.h"
#define TAG "m_hmi"
/***************Variable***************/
static hmi_handle_t hmihandle;
static hmi_callback_t hmi_callback;

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

static void hmiHandleEvent_callback(uint8_t event, uint32_t value)
{
    if (NULL != hmi_callback)
    {
        hmi_callback(event, value);
    }
#if (Enabled == PRINTF_ERR)
    else
    {
        OB_LOGE(TAG, "Err: hmi callback is null");
    }
#endif
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
        case HMI_STATE_KEY_BOARD_WAKE_UP:
            hmi_logo_led_config(LOGO_LED_COLOR_BLUE, LOGO_LED_COLOR_IDLE, HMI_STATE_KEEP_TIME_2s, 1);
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
            hmi_logo_led_config(LOGO_LED_COLOR_BLUE, LOGO_LED_COLOR_IDLE, 0, 0);
            hmi_key_board_led_config(TRUN_ON, TRUN_ON, 0, 0);
            break;
        case HMI_STATE_VERIFY_SUCCESS:
            hmi_logo_led_config(LOGO_LED_COLOR_GREEN, LOGO_LED_COLOR_IDLE, HMI_STATE_KEEP_TIME_2s, 1);
            hmi_key_board_led_config(TRUN_OFF, TRUN_OFF, 0, 0);
            keepTime = 6000;
            break;
        case HMI_STATE_DEMO_VERIFY_SUCCESS:
            hmi_logo_led_config(LOGO_LED_COLOR_RED, LOGO_LED_COLOR_GREEN, HMI_STATE_KEEP_TIME_2s, 2);
            hmi_key_board_led_config(TRUN_OFF, TRUN_OFF, 0, 0);
            keepTime = 6000;
            break;
        case HMI_STATE_LOW_PWOER_VERIFY_SUCCESS:
            hmi_logo_led_config(LOGO_LED_COLOR_RED, LOGO_LED_COLOR_GREEN, HMI_STATE_KEEP_TIME_1s, 2);
            hmi_key_board_led_config(TRUN_OFF, TRUN_OFF, 0, 0);
            keepTime = 6000;
            break;

        case HMI_STATE_RESET_SUCCESS:
            hmi_logo_led_config(LOGO_LED_COLOR_GREEN, LOGO_LED_COLOR_IDLE, HMI_STATE_KEEP_TIME_2s, 1);
            break;
        case HMI_STATE_VERIFY_ADMIN_SUCCESS:
            break;

        case HMI_STATE_VERIFY_FAIL:
            hmi_logo_led_config(LOGO_LED_COLOR_RED, LOGO_LED_COLOR_IDLE, HMI_STATE_KEEP_TIME_100ms, 4);
            hmi_key_board_led_config(TRUN_ON, TRUN_OFF, HMI_STATE_KEEP_TIME_100ms, 4);
            // if (!silentFlag)
            //     PLAYER_LIST_CLEAR_ADD(VOICE_Verification_failed);
            keepTime = HMI_STATE_KEEP_TIME_1s;
            break;

        case HMI_STATE_INPUT_ERROR:
            hmi_logo_led_config(LOGO_LED_COLOR_RED, LOGO_LED_COLOR_IDLE, HMI_STATE_KEEP_TIME_100ms, 4);
            hmi_key_board_led_config(TRUN_ON, TRUN_OFF, HMI_STATE_KEEP_TIME_100ms, 4);
            if (!silentFlag)
                PLAYER_LIST_CLEAR_ADD(VOICE_Input_error);
            keepTime = HMI_STATE_KEEP_TIME_1s;
            break;

        case HMI_STATE_ENTER_ADMIN_MODE:
            hmi_key_board_led_config(TRUN_ON, TRUN_ON, 0, 0);
            if (!silentFlag)
                PLAYER_LIST_CLEAR_ADD(VOICE_Entered_management_mode);
            break;

        case HMI_STATE_VERIFY_ADMIN_CODE:
            hmi_key_board_led_config(TRUN_ON, TRUN_ON, 0, 0);
            if (!silentFlag)
                PLAYER_LIST_ADD(VOICE_Please_enter_a_6_to_12_digit_master_PIN_code);
            break;

        case HMI_STATE_ADMIN:
            hmi_key_board_led_config(TRUN_ON, TRUN_ON, 0, 0);
            PLAYER_LIST_CLEAR_ADD(VOICE_One);
            PLAYER_LIST_ADD(VOICE_Two);
            break;
        
        case HMI_STATE_CHANGE_MASTER_CODE:
            hmi_key_board_led_config(TRUN_ON, TRUN_ON, 0, 0);
            PLAYER_LIST_ADD(VOICE_Please_enter_a_6_to_12_digit_master_PIN_code,VOICE_End_with_pound_key);
            break;

        case HMI_STATE_PIN_CODE_TOO_SIMPLE:
            PLAYER_LIST_CLEAR_ADD(VOICE_PIN_code_is_too_simple,VOICE_Please_re_enter);
            break;

        case HMI_STATE_USER_SETTINGS:
            PLAYER_LIST_CLEAR_ADD(VOICE_To_change_the_master_PIN_code_please_press,VOICE_One,VOICE_To_add_a_user_please_press,VOICE_Two);
            break;

        case HMI_STATE_ADD_NORMAL_USER:
        {
            uint16_t user_id = read_user_id();
            PLAYER_LIST_CLEAR_ADD(VOICE_User_number);
            PLAYER_LIST_ADD(VOICE_Zero + user_id / 100);
            PLAYER_LIST_ADD(VOICE_Zero + (user_id / 10) % 10);
            PLAYER_LIST_ADD(VOICE_Zero + user_id % 10);
            break;
        }

        case HMI_STATE_SYSTEM_SETTINGS:
            PLAYER_LIST_CLEAR_ADD(VOICE_One,VOICE_Create_linked_unlocking_please_press,VOICE_Two);
            break;

        case HMI_STATE_LANGAGESETTING:
            PLAYER_LIST_CLEAR_ADD(VOICE_One,VOICE_Two);
            break;

        case HMI_STATE_REPEAT_INPUT_CODE:
            PLAYER_LIST_CLEAR_ADD(VOICE_Please_enter_again,VOICE_End_with_pound_key);
            break;

        case HMI_STATE_INPUT_ERROR_AGAIN:
            PLAYER_LIST_CLEAR_ADD(VOICE_Input_error,VOICE_Please_re_enter);
            break;

        case HMI_STATE_PIN_REPEAT:
            PLAYER_LIST_CLEAR_ADD(VOICE_PIN_code_already_exists,VOICE_Please_re_enter);
            break;

        case HMI_STATE_CARD_REPEAT:
            PLAYER_LIST_CLEAR_ADD(VOICE_Addition_failed, VOICE_Key_tag_already_exists);
            keepTime = HMI_STATE_KEEP_TIME_2s;
            break;

        case HMI_STATE_PIN_DIFFERENT:
            PLAYER_LIST_CLEAR_ADD(VOICE_Addition_failed,VOICE_PIN_codes_entered_do_not_match);
            break;

        case HMI_STATE_HANDLE_ADD_SUCCESS:
            PLAYER_LIST_CLEAR_ADD(VOICE_Addition_successful);
            break;

        case HMI_STATE_ENROLL_FINGER_PRESS:
            PLAYER_LIST_CLEAR_ADD(SOUND_BUTTON_DI, VOICE_Please_remove_your_finger_and_press_again);
            break;
        
        case HMI_STATE_ENROLLMENT_FAIL:
            PLAYER_LIST_CLEAR_ADD(VOICE_Addition_failed);
            break;

        case HMI_STATE_HANDLE_VOICE_SUCCESS:
            if (readUserParameter(USER_PARA_SILENT_MODE_ID)){
                hmi_logo_led_config(LOGO_LED_COLOR_GREEN, LOGO_LED_COLOR_IDLE, HMI_STATE_KEEP_TIME_1s, 1);
                PLAYER_LIST_CLEAR_ADD(VOICE_Voice_mode);
            }
            else{
                hmi_logo_led_config(LOGO_LED_COLOR_BLUE, LOGO_LED_COLOR_IDLE, HMI_STATE_KEEP_TIME_1s, 1);
                PLAYER_LIST_CLEAR_ADD(VOICE_Mute_mode);
            }
            break;
        
        case HMI_STATE_TAMPER_WARN:
            PLAYER_LIST_CLEAR_ADD(SOUND_WARN,SOUND_WARN,SOUND_WARN);
            break;

        case HMI_STATE_HANDLE_SUCCESS:
            PLAYER_LIST_CLEAR_ADD(VOICE_Setup_successful);
            break;

        case HMI_STATE_CREATE_LINKED_UNLOCK:
            PLAYER_LIST_ADD(VOICE_Please_enter_a_random_4_digit_pairing_code, VOICE_End_with_pound_key);
            break;
        
        case HMI_STATE_JOIN_LINKED_UNLOCK:
            PLAYER_LIST_ADD(VOICE_Please_enter_a_4_digit_pairing_code, VOICE_End_with_pound_key);
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

//  @brief 注册回调函数
//  @param callback
void hmiEventRegister_callback(hmi_callback_t callback)
{
    hmi_callback = callback;
}

void module_hmi_tamper_warn_time(uint32_t warn_time)
{
    hmihandle.tamper.cnt = (warn_time / TAMPER_WARN_PERIOD_TIME);
    if (warn_time)
    {
        hmihandle.tamper.timeOut = system_inc_time_cnt(1500);
        hmihandle.tamper.busy = true;
    }
    else
    {
        hmihandle.tamper.busy = false;
        hmihandle.tamper.cnt = 0;
    }
}

static void hmi_break_warn_loop(void)
{
    if (hmihandle.tamper.cnt){
        if (system_out_time_cnt(hmihandle.tamper.timeOut))
        {
            hmihandle.tamper.cnt--;
            if (hmihandle.tamper.cnt == 0)
                setUserParameter(USER_PARA_BREAK_ID, Disabled);
            hmihandle.tamper.timeOut = system_inc_time_cnt(TAMPER_WARN_PERIOD_TIME);
            if (!play_task_is_busy())
            {
                hmiHandleEvent_callback(HMI_STATE_TAMPER_WARN,0);

            }
        }
        
    }
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

    // 防撬报警逻辑
    hmi_break_warn_loop();
}
