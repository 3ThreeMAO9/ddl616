#include "OB90A64M1.h"
#include "wdt.h"

extern void wdt_IRQ_callback(void);

void WDT_IRQHandler(void)
{
    // Reload WWDT counter and clear WWDT interrupt flag
    wdt_IRQ_callback();
    // WDT_ReloadCounter();
    WDT_ClearIntFlag();
}

void WDT_ReloadCounter(void)
{
    OB_WWDT->WWDTRLD = 0x00005AA5;
}

void WDT_ClearIntFlag(void)
{
    OB_WWDT->WWDTSR_b.WWDTIF = 1;
}

void WDT_Open(uint32_t nPreScale, uint32_t nCmpValue, uint32_t nEnableInt)
{
    OB_WWDT->WWDTRLD = 0x6996C33C;
    OB_WWDT->WWDTCR =  0x00;
    if(nCmpValue < 0x02)  // min = 2
        nCmpValue = 0x02;

    if (nEnableInt == 0x01)
    {
        OB_WWDT->WWDTCR_b.WWDTIE    = 1;
        OB_WWDT->WWDTCR_b.PERIODSEL = nPreScale;
        OB_WWDT->WWDTCR_b.WINCMP    = nCmpValue;
        NVIC_EnableIRQ(WDT_IRQn);
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
    OB_WWDT->WWDTCR = 0x00;
    //WDT频率较慢，所以需要等一小段时间
    Loop = 100;
    while (Loop--);
}

