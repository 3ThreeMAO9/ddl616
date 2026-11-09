#ifndef __SYSTEM_OB90A64M1_H
#define __SYSTEM_OB90A64M1_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
    
#define TRUE    1
#define FALSE   0

/**
 * System core clock
 */
extern volatile uint32_t SystemCoreClock;

/**
 * Initialize the system
 *
 * @param  none
 * @return none
 *
 * @brief  Setup the microcontroller system.
 *         Initialize the System and update the SystemCoreClock variable.
 */
extern void SystemInit (void);

/**
 * Update SystemCoreClock variable
 *
 * @param  none
 * @return none
 *
 * @brief  Updates the SystemCoreClock with current core Clock 
 *         retrieved from cpu registers.
 */
extern void SystemCoreClockUpdate (void);


/**
 * Get Peripheral Clock
 *
 * @param  nAPBModuleIndex
 * @return Peripheral Clock
 *
 * @brief  Get peripheral clock.
 */
#define APB_SPI     0
#define APB_CORDIC  2
#define APB_QEI     4
#define APB_RTC     6
#define APB_DMA     8 
#define APB_MDU     10 
#define APB_ADC0    12 
#define APB_ADC1    14
#define APB_UART0   16
#define APB_WWDT    18
#define APB_I2C0    20
#define APB_UART1   22
#define APB_PWM     24
#define APB_I2C1    26
#define APB_T16B0   28
#define APB_T16B1   30
#define APB_T32B0   0
#define APB_T32B1   2

extern uint32_t GetPeripheralClock(uint32_t nAPBModuleIndex);
extern void     WaitingCPUReady(void);


#ifdef __cplusplus
}
#endif

#endif /* __SYSTEM_OB90A64M1_H */
