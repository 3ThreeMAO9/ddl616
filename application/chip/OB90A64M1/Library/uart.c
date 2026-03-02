#include <stdio.h>
#include "OB90A64M1.h"
#include "uart.h"
#include "hal_uart.h"
#include "soft_uart.h"

volatile uint8_t  g_UartRxData[UART_RX_SIZE];
volatile uint32_t g_UartRxCounter = 0x00;
volatile uint32_t g_UartInterrupt = 0x00;
OB_UART_Type* g_uart_par = OB_UART0;

static uart_callback_t uart0_callback;
static uart_callback_t uart1_callback;

extern void hal_uart_receive_buff_callback(Uart_Group_t uart_group, uint8_t data);

void UART0_IRQHandler(void)
{
    if ((OB_UART0->ULSR & UART_INT_RBR) == UART_INT_RBR)
    {
        if (NULL != uart0_callback)
            uart0_callback(OB_UART0->URBR);
        hal_uart_receive_buff_callback(UART_0, OB_UART0->URBR);

    }
    else if ((OB_UART0->ULSR & UART_INT_TEMT) == UART_INT_TEMT)
    {
        UART_DisableInt(OB_UART0, UART_INT_THRE);
    }
}

void UART1_IRQHandler(void)
{
    if ((OB_UART1->ULSR & UART_INT_RBR) == UART_INT_RBR)
    {
        if (NULL != uart1_callback)
            uart1_callback(OB_UART1->URBR);
        hal_uart_receive_buff_callback(UART_1, OB_UART1->URBR);
    }
    else if ((OB_UART1->ULSR & UART_INT_TEMT) == UART_INT_TEMT)
    {
        UART_DisableInt(OB_UART1, UART_INT_THRE);
    }
}

void UART_DisableInt(OB_UART_Type* pUart, uint32_t nInterruptFlag )
{
    pUart->UIER &= ~nInterruptFlag;
}



void UART_EnableInt(OB_UART_Type* pUart, uint32_t nInterruptFlag )
{
    pUart->UIER = nInterruptFlag;
}


void UART_Open(OB_UART_Type* pUart, uint32_t nBaudRate, uart_callback_t callback)
{
    uint32_t nClock, nBuff;
    if (pUart == OB_UART0){
        nClock = GetPeripheralClock(APB_UART0);
        uart0_callback = callback;
    }
    else{
        nClock = GetPeripheralClock(APB_UART1);
        uart1_callback = callback;
    }

    nBuff = nClock / nBaudRate ;
    pUart->UDL = nBuff;
    pUart->ULCR = UART_WORD_LEN_8 | UART_STOP_BIT_1 | UART_PARITY_NONE;
}

uint32_t UART_Read(OB_UART_Type* pUart, uint8_t *pRxBuf, uint32_t nReadBytes)
{
    uint32_t nCount, nDelay;

    for (nCount = 0; nCount < nReadBytes; nCount++)
    {
        nDelay = 0x04000000;
        while(1)
        {
            if (nDelay == 0x00)
                return 0x00;

            if ((pUart->ULSR & UART_STATUS_RDR) == UART_STATUS_RDR)
            {
                pRxBuf[nCount] = pUart->URBR;
                break;
            }
            nDelay--;
        }
    }
    return nReadBytes;
}

uint32_t UART_Write(OB_UART_Type* pUart, uint8_t *pTxBuf, uint32_t nWriteBytes)
{
    uint32_t nCount, nDelay;

    for (nCount = 0; nCount < nWriteBytes; nCount++)
    {
        pUart->UTHR = pTxBuf[nCount];

        nDelay = 0x04000000;
        while(1)
        {
            if (nDelay == 0x00)
                return 0x00;

            if ((pUart->ULSR & UART_STATUS_TEMT) == UART_STATUS_TEMT)
                break;
            nDelay--;
        }
    }
    return nWriteBytes;
}


/*******************************************************************************
* Function Name  :  int fputc(int ch, FILE *f)
* Description    :
* Input          : int ch, FILE *f
* Output         :
* Return         : int ch
*******************************************************************************/
int fputc(int ch, FILE *f)
{
#if (OB_LOG_UART_SIMU == OB_LOG_UART_SEL)
    simu_putchar((uint8_t)ch);
    return 1;
#else
    uint8_t Buff;
    Buff = (uint8_t)ch;
    return SoftUART_LogWrite(g_uart_par, &Buff, 1);
#endif
}
/*******************************************************************************
   Function Name  : fputc
   Description        : redefine getc function,use scanf with uart input
   Input                  : FILE *f
   Output               :
* Return               : int ch
********************************************************************************/
int fgetc(FILE *f)
{
    uint8_t Buff;
    UART_Read(g_uart_par, &Buff, 1);
    return (int)Buff;
}
