#include "led.h"
#include "task_key.h"
#include "hal_wdt.h"
#define OB_LOG_LEVEL OB_LOG_LEVEL_DEBUG
#include "ob_log.h"
#define TAG "led"

/***************Variable***************/


// ------------------------------------------
void ledPowerOnInit(void)
{
    RED_LED_INIT();
    GREEN_LED_INIT();
    BLUE_LED_INIT();
    WHITE_LED_INIT();
    logoLedDrive(LOGO_LED_COLOR_BLUE);
    keyBoardLedDrive(TRUN_OFF);
}

void ledInit(void)
{
#if (Enabled==PRINTF_HMI)
    OB_LOGD(TAG,"ledInit");
#endif
    RED_LED_INIT();
    GREEN_LED_INIT();
    BLUE_LED_INIT();
    WHITE_LED_INIT();
    
    logoLedDrive(LOGO_LED_COLOR_IDLE);
    // keyBoardLedDrive(TRUN_ON);
}

void logoLedDrive(uint8_t color)
{
    static uint8_t colorLast = LOGO_LED_COLOR_IDLE;

    color &= 0x0F;

    if(colorLast == color)
    {
        return;
    }
#if (Enabled==PRINTF_HMI)
    OB_LOGD(TAG,"logoLedDrive[%u]", color);
#endif
    colorLast = color;

    switch (color)
    {
        case LOGO_LED_COLOR_RED:
            LOGO_RED_LED_OPEN();
            break;
        case LOGO_LED_COLOR_GREEN:
            LOGO_GREEN_LED_OPEN();
            break;
        case LOGO_LED_COLOR_BLUE:
            LOGO_BLUE_LED_OPEN();
            break;
        default:
            LOGO_LED_CLOSE();
            break;
    }
}

void keyBoardLedDrive(uint8_t ledSw)
{
    static uint8_t LedSwLast = TRUN_OFF;

    ledSw &= 0x0F;
    if(LedSwLast == ledSw)
    {
        return;
    }
#if (Enabled==PRINTF_HMI)
    OB_LOGD(TAG,"keyBoardLedDrive[%u]", ledSw);
#endif
    LedSwLast = ledSw;

    if(TRUN_ON == ledSw)
    {
        SET_WHITE_LED_OPEN();
    }
    else
    {
        SET_WHITE_LED_CLOSE();
    }
}
