/**
 * @file startup.c
 * @author Dandjinh Deng (Dandjinh_Deng@on-bright.com)
 * @brief 
 * @version 0.1
 * @date 2024-08-16
 * 
 * @copyright Copyright (c) 2024 广州昂宝电子有限公司
 * 
 */

#include "OB90A64M1.h"
#include "chip_config.h"

#define __VECTOR_TABLE_ATTRIBUTE __attribute__((section(".ARM.__at_0x0000FD40"), used))
typedef void(*VECTOR_TABLE_Type)(void);

__NO_RETURN void Reset_Handler  (void);
__NO_RETURN void Default_Handler(void);

void NMI_Handler            (void) __attribute__ ((weak, alias("Default_Handler")));
void HardFault_Handler      (void) __attribute__ ((weak, alias("Default_Handler")));
void SVC_Handler            (void) __attribute__ ((weak, alias("Default_Handler")));
void PendSV_Handler         (void) __attribute__ ((weak, alias("Default_Handler")));
void SysTick_Handler        (void) __attribute__ ((weak, alias("Default_Handler")));
void I2C1_IRQHandler        (void) __attribute__ ((weak, alias("Default_Handler")));
void LVI_IRQHandler         (void) __attribute__ ((weak, alias("Default_Handler")));
void ACMP_IRQHandler        (void) __attribute__ ((weak, alias("Default_Handler")));
void RTC_IRQHandler         (void) __attribute__ ((weak, alias("Default_Handler")));
void I2C0_IRQHandler        (void) __attribute__ ((weak, alias("Default_Handler")));
void CT16B0_IRQHandler      (void) __attribute__ ((weak, alias("Default_Handler")));
void CT16B1_IRQHandler      (void) __attribute__ ((weak, alias("Default_Handler")));
void CT32B0_IRQHandler      (void) __attribute__ ((weak, alias("Default_Handler")));
void CT32B1_IRQHandler      (void) __attribute__ ((weak, alias("Default_Handler")));
void SPI_IRQHandler         (void) __attribute__ ((weak, alias("Default_Handler")));
void UART0_IRQHandler       (void) __attribute__ ((weak, alias("Default_Handler")));
void QEI_IRQHandler         (void) __attribute__ ((weak, alias("Default_Handler")));
void PDMA_IRQHandler        (void) __attribute__ ((weak, alias("Default_Handler")));
void PWM_IRQHandler         (void) __attribute__ ((weak, alias("Default_Handler")));
void WDT_IRQHandler         (void) __attribute__ ((weak, alias("Default_Handler")));
void ADC_IRQHandler         (void) __attribute__ ((weak, alias("Default_Handler")));
void UART1_IRQHandler       (void) __attribute__ ((weak, alias("Default_Handler")));
void GPIO0_IRQHandler       (void) __attribute__ ((weak, alias("Default_Handler")));
void GPIO1_IRQHandler       (void) __attribute__ ((weak, alias("Default_Handler")));
void GPIO2_IRQHandler       (void) __attribute__ ((weak, alias("Default_Handler")));
void GPIO3_IRQHandler       (void) __attribute__ ((weak, alias("Default_Handler")));
void PWMTM_IRQHandler       (void) __attribute__ ((weak, alias("Default_Handler")));

extern const VECTOR_TABLE_Type __VECTOR_TABLE[48];
       const VECTOR_TABLE_Type __VECTOR_TABLE[48] __VECTOR_TABLE_ATTRIBUTE = {
    (VECTOR_TABLE_Type)(_RAM_BEGIN_ADDR + _RAM_MAX_SIZE),
        Reset_Handler,               // Reset Handler
    NMI_Handler,                 // NMI Handler
    HardFault_Handler,           // Hard Fault Handler
    0,                           // Reserved
    0,                           // Reserved
    0,                           // Reserved
    0,                           // Reserved
    0,                           // Reserved
    0,                           // Reserved
    0,                           // Reserved
    SVC_Handler,                 // SVCall Handler
    0,                           // Reserved
    0,                           // Reserved
    PendSV_Handler,              // PendSV Handler
    SysTick_Handler,             // SysTick Handler

    // External Interrupts
    0,                           // 16+ 0,: Reserved
    I2C1_IRQHandler,             // 16+ 1: I2C1
    LVI_IRQHandler,              // 16+ 2: LVI
    ACMP_IRQHandler,             // 16+ 3: ACMP
    RTC_IRQHandler,              // 16+ 4: RTC
    I2C0_IRQHandler,             // 16+ 5: I2C0,
    CT16B0_IRQHandler,           // 16+ 6: CT16B0,
    CT16B1_IRQHandler,           // 16+ 7: CT16B1
    CT32B0_IRQHandler,           // 16+ 8: CT32B0,
    CT32B1_IRQHandler,           // 16+ 9: CT32B1
    SPI_IRQHandler,              // 16+10,: SPI
    UART0_IRQHandler,            // 16+11: UART0,
    QEI_IRQHandler,              // 16+12: QEI
    PDMA_IRQHandler,             // 16+13: PDMA
    PWM_IRQHandler,              // 16+14: PWM
    WDT_IRQHandler,              // 16+15: WDT
    ADC_IRQHandler,              // 16+16: ADC
    UART1_IRQHandler,            // 16+17: UART1
    GPIO0_IRQHandler,            // 16+18: GPIO0,
    GPIO1_IRQHandler,            // 16+19: GPIO1
    GPIO2_IRQHandler,            // 16+20,: GPIO2
    GPIO3_IRQHandler,            // 16+21: GPIO3
    PWMTM_IRQHandler,            // 16+22: PWMTM
};
extern const uint32_t g_vector_table_size;
const uint32_t g_vector_table_size = sizeof(__VECTOR_TABLE);

__NO_RETURN void Reset_Handler(void)
{
    extern __NO_RETURN void boot_main(void);
    boot_main();
}

__NO_RETURN
void Default_Handler(void)
{
  while(1);
}
