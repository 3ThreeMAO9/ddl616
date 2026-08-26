#include "simu_uart.h"
#include "timer16.h"


/***************Variable***************/


// ------------------------------------------

// baudrate:115200
void delay_simu_uart(void)
{
    for (uint8_t i = 0; i < 32; i++);
}

void simu_uart_init(void) {
   SIMU_UART_INIT(1);
}

uint8_t simu_putchar (uint8_t s_data) {
	SIMU_UART_INIT(1);
    TIMER16_Close(OB_CT16B0);

    OUT_START_BIT;
    delay_simu_uart();
    {
		if (s_data&(0x01))
            OUT_HIGH_BIT;
		else
			OUT_LOW_BIT;
		delay_simu_uart();

		if (s_data&(0x02))
			OUT_HIGH_BIT;
		else
			OUT_LOW_BIT;
		delay_simu_uart();

		if (s_data&(0x04))
			OUT_HIGH_BIT;
		else
			OUT_LOW_BIT;
		delay_simu_uart();

		if (s_data&(0x08))
			OUT_HIGH_BIT;
		else
			OUT_LOW_BIT;
		delay_simu_uart();

		if (s_data&(0x10))
			OUT_HIGH_BIT;
		else
			OUT_LOW_BIT;
		delay_simu_uart();

		if (s_data&(0x20))
			OUT_HIGH_BIT;
		else
			OUT_LOW_BIT;
		delay_simu_uart();

		if (s_data&(0x40))
			OUT_HIGH_BIT;
		else
			OUT_LOW_BIT;
		delay_simu_uart();

		if (s_data&(0x80))
			OUT_HIGH_BIT;
		else
			OUT_LOW_BIT;
		delay_simu_uart();
    }
    OUT_STOP_BIT;
    delay_simu_uart();
    delay_simu_uart();
    TIMER16_Enable(OB_CT16B0);

	return 0;
}
