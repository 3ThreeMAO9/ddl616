#ifndef __WDT_H
#define __WDT_H


void WDT_Open(uint32_t nPreScale, uint32_t nCmpValue, uint32_t nEnableInt);
void WDT_ClearIntFlag(void);
void WDT_ReloadCounter(void);
void WDT_Close(void);

#define WDT_INTERRUPT_FLAG      (0x01 << 0x00)

#define WDT_ICE_ENABLE          0x80000000
#define WDT_INTERRUPT_ENABLE    (0x01 << 0x01)
#define WDT_FUNCTIOM_ENABLE     (0x01 << 0x00)


#define WDT_PRESCALER_1         0x00
#define WDT_PRESCALER_2         0x01
#define WDT_PRESCALER_4         0x02
#define WDT_PRESCALER_8         0x03
#define WDT_PRESCALER_16        0x04
#define WDT_PRESCALER_32        0x05
#define WDT_PRESCALER_64        0x06
#define WDT_PRESCALER_128       0x07
#define WDT_PRESCALER_192       0x08
#define WDT_PRESCALER_256       0x09
#define WDT_PRESCALER_384       0x0A
#define WDT_PRESCALER_512       0x0B
#define WDT_PRESCALER_768       0x0C
#define WDT_PRESCALER_1024      0x0D
#define WDT_PRESCALER_1356      0x0E
#define WDT_PRESCALER_2048      0x0F

#endif
