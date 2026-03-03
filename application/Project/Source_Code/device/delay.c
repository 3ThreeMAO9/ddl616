#include "delay.h"

/*********************Variable******************/

// --------------------------------------------
void delay_us(uint16_t n)
{
    uint16_t i,j;

    for (i = 0; i < n; i++)
    {
        for (j = 0; j < 5; j++);
    }
}

void delay_ms(uint16_t n)   // 1ms = 0.92ms
{
    uint16_t i;

    for (i = 0; i < n; i++)
    {
        delay_us(1000);
    }
}
