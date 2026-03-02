#include "hal_wdt.h"

#define OB_LOG_LEVEL OB_LOG_LEVEL_NONE
#include "ob_log.h"
#define TAG "hal_wdt"

/***********Variable***********/
static volatile uint8_t WdtIrqCnt;
static wdt_callback_t wdt_callback;
// ----------------------------

void wdt_IRQ_callback(void)
{
    if (WdtIrqCnt < 10)
    {
        FEED_DOG();
    }
    WdtIrqCnt++;

    if (NULL != wdt_callback)
    {
        wdt_callback();
    }
}

void clear_feed_dog_cnt(void)
{
    WdtIrqCnt = 0;
}

void hal_wdt_init(wdt_callback_t callback)
{
    WDT_Open(WDT_PRESCALER_1024, 0x2D, 1);
    WdtIrqCnt = 0;
    wdt_callback = callback;
}

void hal_wdt_feed_dog(void)
{
    FEED_DOG();
}


