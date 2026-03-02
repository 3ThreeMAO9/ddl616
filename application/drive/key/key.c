#include "key.h"
#include "key_board.h"
#include "tamper_key.h"
#include "hal_timer.h"
#include "system_timer.h"

#define OB_LOG_LEVEL OB_LOG_LEVEL_NONE
#include "ob_log.h"
#define TAG "key"

/*************************Variable*************************/
static key_handle_t keyHandle;
static key_event_callback_t key_callback;

uint8_t touch_sensity[TOUCH_CH_CNT] ={
    TOUCH_IC_CH0,
    TOUCH_IC_CH1,
    TOUCH_IC_CH2,
    TOUCH_IC_CH3,
    TOUCH_IC_CH4,
    TOUCH_IC_CH5,
    TOUCH_IC_CH6,
    TOUCH_IC_CH7,
    TOUCH_IC_CH8,
    TOUCH_IC_CH9,
    TOUCH_IC_CH10,
    TOUCH_IC_CH11,
    TOUCH_IC_CH12,
    TOUCH_IC_CH13,
};

// ---------------------------------------------------------
// @brief key事件回调
// @param keyType，按键类型
// @param keyEvent，按键事件
// @return true - sucess; false - fail
// ---------------------------------------------------------
static void keyEvent_callback(uint8_t keyType, uint8_t keyEvent)
{
    if (NULL != key_callback)
    {
        key_callback(keyType, keyEvent);
    }
#if (Enabled == PRINTF_ERR)
    else
    {
        OB_LOGE(TAG, "Err:key callback is null");
    }
#endif
}

static void tamper_key_handler_callback(void* btn)
{
    struct Button* button = (Button*)btn;
    switch (button->event)
    {
        case BTN_PRESS_DOWN:
            OB_LOGD(TAG,"BTN_PRESS_DOWN");
            keyHandle.tamper_key.busy = true;
            keyEvent_callback(KEY_TYPE_TAMPER_KEY, KEY_EVENT_TAMPER_PRESS_DOWN);
            break;

        case BTN_PRESS_UP:
            OB_LOGD(TAG,"BTN_PRESS_UP");
            keyHandle.tamper_key.pressCnt = 0;
            keyEvent_callback(KEY_TYPE_TAMPER_KEY, KEY_EVENT_TAMPER_PRESS_UP);
            break;

        case BTN_LONG_PRESS_START:
            OB_LOGD(TAG,"LONG PRESS");
            keyHandle.tamper_key.busy = false;
            keyEvent_callback(KEY_TYPE_TAMPER_KEY, KEY_EVENT_TAMPER_PRESS);
            break;

        default:
            break;
    }
}

static void keyBoardHandler_callback(void* btn)
{
    struct Button* button = (Button*)btn;
    static uint8_t long_press_hold_flag = 0;

    switch (button->event)
    {
        case BTN_PRESS_DOWN:
            long_press_hold_flag =false;
            OB_LOGD(TAG,"BTN_PRESS_DOWN[%u]", button->button_id);
            if (keyHandle.keyBoard.pressCnt < KEY_BOARD_LONG_PRESS_TIME)
            {
                keyEvent_callback(KEY_TYPE_KEY_BOARD, button->button_id);
            }
            keyHandle.keyBoard.pressCnt = 0;
            keyHandle.keyBoard.busy = true;
            break;

        case BTN_PRESS_UP:
            OB_LOGD(TAG,"BTN_PRESS_UP[%u]", button->button_id);
            keyHandle.keyBoard.busy = false;
            keyHandle.keyBoard.pressCnt = 0;
            if(long_press_hold_flag)
            {
                long_press_hold_flag =false;
                keyEvent_callback(KEY_TYPE_LONG_RELEASE, button->button_id);
            }
            break;

        case BTN_LONG_PRESS_HOLD:
            if (keyHandle.keyBoard.pressCnt < KEY_BOARD_LONG_PRESS_TIME)
            {
                keyHandle.keyBoard.pressCnt++;
                if (keyHandle.keyBoard.pressCnt >= KEY_BOARD_LONG_PRESS_TIME)
                {
                    OB_LOGD(TAG,"LONG PRESS");
                    keyHandle.keyBoard.busy = false;
                    long_press_hold_flag = true;
                    keyEvent_callback(KEY_TYPE_KEY_BOARD, KEY_LOCK);
                }
            }
            break;

        default:
            break;
    }
}

void module_keySwitch(uint8_t type, uint8_t flag)
{
    uint8_t i;
    if (flag)
    {
        switch (type)
        {
            case KEY_TYPE_TAMPER_KEY:
                button_start(&keyHandle.keyBtn[KEY_CNT]);
                break;
            case KEY_TYPE_KEY_BOARD:
                for (i=0; i<KEY_CNT; i++)
                {
                    button_start(&keyHandle.keyBtn[i]);
                }
                break;
            default:
                return;
        }
    }
    else
    {
        switch (type)
        {
            case KEY_TYPE_TAMPER_KEY:
                button_stop(&keyHandle.keyBtn[KEY_CNT]);
                keyHandle.tamper_key.busy = false;
                break;
            case KEY_TYPE_KEY_BOARD:
                for (i=0; i<KEY_CNT; i++)
                {
                    button_stop(&keyHandle.keyBtn[i]);
                }
                keyHandle.keyBoard.busy = false;
                break;
            default:
                return;
        }
    }
    OB_LOGD(TAG,"key type[%u]: flag[%u]", type, flag);
}

static void tamper_key_buttons_init(void)
{
    button_init(&keyHandle.keyBtn[KEY_CNT], read_tamper_key_level, false, KEY_CNT);
    button_attach(&keyHandle.keyBtn[KEY_CNT], BTN_PRESS_DOWN, tamper_key_handler_callback);
    button_attach(&keyHandle.keyBtn[KEY_CNT], BTN_PRESS_UP, tamper_key_handler_callback);
    button_attach(&keyHandle.keyBtn[KEY_CNT], BTN_LONG_PRESS_START, tamper_key_handler_callback);
}

static void keyBoardButtonsInit(void)
{
    uint8_t i;

    for (i=0; i<KEY_CNT; i++)
    {
        button_init(&keyHandle.keyBtn[i], read_keyboard_level, true, i);
        button_attach(&keyHandle.keyBtn[i], BTN_PRESS_DOWN, keyBoardHandler_callback);
        button_attach(&keyHandle.keyBtn[i], BTN_PRESS_UP, keyBoardHandler_callback);
        button_attach(&keyHandle.keyBtn[i], BTN_LONG_PRESS_HOLD, keyBoardHandler_callback);
    }
}

uint8_t module_keyInit(uint8_t type)
{
    OB_LOGD(TAG, "[%s] type = %d", __func__, type);
    switch (type)
    {
        case KEY_TYPE_TAMPER_KEY:
            keyHandle.tamper_key.pressCnt = 0;
            keyHandle.tamper_key.busy = false;

            if (false == keyHandle.tamper_key.enable)
            {
                keyHandle.tamper_key.enable = true;
                tamper_key_init();
                tamper_key_buttons_init();
                module_keySwitch(KEY_TYPE_TAMPER_KEY, true);
            }

            break;

        case KEY_TYPE_KEY_BOARD:
            keyboard_init(((void*)touch_sensity));
            keyHandle.keyBoard.busy = false;
            keyHandle.keyBoard.pressCnt = 0;
            keyHandle.keyBoard.value = KEY_NULL;

            if (false == keyHandle.keyBoard.enable)
            {
                keyHandle.keyBoard.enable = true;
                keyBoardButtonsInit();
                module_keySwitch(KEY_TYPE_KEY_BOARD, true);
            }
            break;

        default:
            return false;
    }
    return true;
}

// @brief 注册回调函数
// @param callback
void keyEventRegister_callback(key_event_callback_t callback)
{
    key_callback = callback;
    OB_LOGD(TAG,"key_register_callback: %X", callback);
}

uint8_t module_keySleep(uint8_t type)
{
    if (KEY_TYPE_TAMPER_KEY == type)
    {
        return true;
    }
    else if (KEY_TYPE_KEY_BOARD == type)
    {
        if (!keyHandle.keyBoard.busy)
        {
            keyboard_sleep_config();
            return true;
        }
    }
#if (Enabled == PRINTF_ERR)
    OB_LOGE(TAG,"Fail: key[%u] sleep", type);
#endif
    return false;
}

uint8_t module_isBusyKey(uint8_t type)
{
    if (KEY_TYPE_TAMPER_KEY == type)
    {
        return keyHandle.tamper_key.busy;
    }
    else if (KEY_TYPE_KEY_BOARD == type)
    {
        return keyHandle.keyBoard.busy;
    }
#if (Enabled == PRINTF_ERR)
    OB_LOGE(TAG,"Fail: key[%u] is busy", type);
#endif
    return false;
}

void module_key_pause(uint32_t value)
{
//    keyboard_csk14t_pause(value);
}

uint8_t module_keyWake(void)
{
    if(is_keyboard_wake())
    {
        return WAKE_SOURCE_KEY_BOARD;
    }
    if(tamper_key_check_wake())
    {
        return WAKE_SOURCE_TAMPER_KEY;
    }


    return WAKE_SOURCE_NULL;
}

void module_keyScanLoop(void)
{
    if (system_out_time_cnt(keyHandle.timeOut))
    {
        keyHandle.timeOut = system_inc_time_cnt(KEY_SCAN_TIME_OUT);
        keyboard_scan();
        button_ticks();
    }
}

void module_keyTestMode(uint8_t mode)
{
//    keyBoard_testmode(mode);
}
