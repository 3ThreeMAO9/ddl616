#include <stdio.h>
#include "uart.h"
#include "system_OB90A64M1.h"

/***************Variable***************/
static uart_callback_t uart0_callback;

// ------------------------------------------

void UART0_IRQHandler(void)
{
    uint8_t value;
    
    if ((OB_UART0->ULSR & UART_INT_RBR) == UART_INT_RBR)
    {
        value = OB_UART0->URBR;
        if (NULL != uart0_callback)
        {
            uart0_callback(value);
        }
    }
    else if ((OB_UART0->ULSR & UART_INT_TEMT) == UART_INT_TEMT)
    {
        UART_DisableInt(OB_UART0, UART_INT_THRE);
    }
}

void UART1_IRQHandler(void)
{
    uint8_t value;
    
    if ((OB_UART1->ULSR & UART_INT_RBR) == UART_INT_RBR)
    {
        value = OB_UART1->URBR;
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
    if (pUart == OB_UART0)
    {
        nClock = GetPeripheralClock(APB_UART0);
        if (NULL == uart0_callback)
        {
            uart0_callback = callback;
        }
    }
    else
    {
        nClock = GetPeripheralClock(APB_UART1);
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
                pRxBuf[nCount] = (uint8_t)pUart->URBR;
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
