#include "OB90A64M1.h"
#include "wdt.h"

/***************Variable***************/
static wdt_handle_t wdt_handle;

// ------------------------------------------

void WDT_IRQHandler(void)
{
    //  Reload WWDT counter and clear WWDT interrupt flag
    //WDT_ReloadCounter();
    if (wdt_handle.callback) {
        wdt_handle.callback();
    }

    wdt_handle.count++;
    if (wdt_handle.count < 40) {
        OB_WWDT->WWDTRLD = 0x00005AA5;
    }

    OB_WWDT->WWDTSR_b.WWDTIF = 1;
}

void WDT_ReloadCounter(void)
{
    if (OB_WWDT->WWDTCR_b.WWDTIE) {
        wdt_handle.count = 0;
    }
    else {
        OB_WWDT->WWDTRLD = 0x00005AA5;
    }
}

void WDT_ClearIntFlag(void)
{
    OB_WWDT->WWDTSR_b.WWDTIF = 1;
}

void WDT_Open(uint32_t nPreScale, uint32_t nCmpValue, uint32_t nEnableInt, wdt_irq_callback_t callback)
{
    OB_WWDT->WWDTRLD = 0x6996C33C;
    OB_WWDT->WWDTCR =  0x00;

    if (nEnableInt == 0x01)
    {
        OB_WWDT->WWDTCR_b.WWDTIE    = 1;
        OB_WWDT->WWDTCR_b.PERIODSEL = nPreScale;
        OB_WWDT->WWDTCR_b.WINCMP    = nCmpValue;
	    NVIC_SetPriority(WDT_IRQn, 1);
        NVIC_EnableIRQ(WDT_IRQn);

        wdt_handle.callback = callback;
        wdt_handle.count = 0;
    }
    else
    {
        NVIC_DisableIRQ(WDT_IRQn);
        OB_WWDT->WWDTCR_b.WWDTIE    = 0;
        OB_WWDT->WWDTCR_b.PERIODSEL = nPreScale;
        OB_WWDT->WWDTCR_b.WINCMP    = nCmpValue;
    }

    OB_WWDT->WWDTCR_b.WWDTEN = 1;

    return;
}

void WDT_Close(void)
{
	uint32_t Loop;
	OB_WWDT->WWDTRLD = 0x6996C33C;
    OB_WWDT->WWDTCR =  0x00;
	//--------/ WDT 頻率 較慢 所以需要等一小段時間  
    Loop = 100;
	while(Loop--);  	
}
