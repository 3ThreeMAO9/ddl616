#include "OB90A64M1.h"
#include "gpio.h"
#include "config.h"

volatile uint32_t g_GPIO_Interrupt[OB_GPIO_CNT] = {0};
extern void SoftUART_RxGPIO_IRQHandler(void);

void GPIO0_IRQHandler(void)
{
    g_GPIO_Interrupt[OB_GPIO0_ID] = OB_GPIO0->IEST;
    OB_GPIO0->IESTC = g_GPIO_Interrupt[OB_GPIO0_ID];
    HAL_GPIO_GetIntState(OB_GPIO0_ID,g_GPIO_Interrupt[OB_GPIO0_ID]);
}

void GPIO1_IRQHandler(void)
{
    g_GPIO_Interrupt[OB_GPIO1_ID] = OB_GPIO1->IEST;
    OB_GPIO1->IESTC = g_GPIO_Interrupt[OB_GPIO1_ID];
    HAL_GPIO_GetIntState(OB_GPIO1_ID,g_GPIO_Interrupt[OB_GPIO1_ID]);
}

void GPIO2_IRQHandler(void)
{
    g_GPIO_Interrupt[OB_GPIO2_ID] = OB_GPIO2->IEST;
    OB_GPIO2->IESTC = g_GPIO_Interrupt[OB_GPIO2_ID];
    HAL_GPIO_GetIntState(OB_GPIO2_ID,g_GPIO_Interrupt[OB_GPIO2_ID]);
    // OB_GPIO2->DATA |= GPIO_PIN3;   //test irq time
    SoftUART_RxGPIO_IRQHandler();
    // OB_GPIO2->DATA &= (~GPIO_PIN3);   //test irq time
}

void GPIO3_IRQHandler(void)
{
    g_GPIO_Interrupt[OB_GPIO3_ID] = OB_GPIO3->IEST;
    OB_GPIO3->IESTC = g_GPIO_Interrupt[OB_GPIO3_ID];
    HAL_GPIO_GetIntState(OB_GPIO3_ID,g_GPIO_Interrupt[OB_GPIO3_ID]);
}
