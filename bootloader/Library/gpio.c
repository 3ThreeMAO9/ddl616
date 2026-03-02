#include "gpio.h"


/***************Function***************/
volatile uint32_t g_GPIO_Interrupt[OB_GPIO_CNT] = {0};

// ------------------------------------------
void GPIO0_IRQHandler(void)
{
    g_GPIO_Interrupt[OB_GPIO0_ID] = OB_GPIO0->IEST;
    OB_GPIO0->IESTC = g_GPIO_Interrupt[OB_GPIO0_ID];
}

void GPIO1_IRQHandler(void)
{
    g_GPIO_Interrupt[OB_GPIO1_ID] = OB_GPIO1->IEST;
    OB_GPIO1->IESTC = g_GPIO_Interrupt[OB_GPIO1_ID];
}

void GPIO2_IRQHandler(void)
{
    g_GPIO_Interrupt[OB_GPIO2_ID] = OB_GPIO2->IEST;
    OB_GPIO2->IESTC = g_GPIO_Interrupt[OB_GPIO2_ID];
}

void GPIO3_IRQHandler(void)
{
    g_GPIO_Interrupt[OB_GPIO3_ID] = OB_GPIO3->IEST;
    OB_GPIO3->IESTC = g_GPIO_Interrupt[OB_GPIO3_ID];
}
