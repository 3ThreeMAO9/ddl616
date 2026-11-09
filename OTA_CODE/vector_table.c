/**
 * @file vector_table.c
 * @author Dandjinh Deng (Dandjinh_Deng@on-bright.com)
 * @brief 
 * @version 0.1
 * @date 2022-08-31
 * 
 * @copyright Copyright (c) 2022 广州昂宝电子有限公司
 * 
 */

#include "OB90A64M1.h"
#include "system_OB90A64M1.h"

#include <stdio.h>
#include "config.h"

#if defined (__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050)
    #pragma clang diagnostic push
    #pragma clang diagnostic ignored "-Wmissing-prototypes"
#endif

#define _IRQ_IMPORT(name)                    \
    case name##_IRQn:                        \
    {                                        \
        extern void name##_IRQHandler(void); \
        name##_IRQHandler();                 \
        break;                               \
    }

__attribute__((used, section("IRQ_ENTRY")))
void irq_switch_handler(int index)
{
    int irq = index;
    irq -= 16;

    switch (irq)
    {
    _IRQ_IMPORT(GPIO0)
    _IRQ_IMPORT(GPIO1)
    _IRQ_IMPORT(GPIO2)
    _IRQ_IMPORT(GPIO3)
    _IRQ_IMPORT(UART0)
    _IRQ_IMPORT(UART1)
    _IRQ_IMPORT(RTC)
    _IRQ_IMPORT(CT16B0)
    _IRQ_IMPORT(CT16B1)
    _IRQ_IMPORT(WDT)
    _IRQ_IMPORT(PWM)
    
    default:
        __disable_irq();
//        LOG("\r\nirq %d\r\n", index);
        while (1)
        {
        }
    }
}

#if defined (__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050)
    #pragma clang diagnostic pop
#endif
