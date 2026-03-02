#ifndef __UART_H 
#define __UART_H
	
#include "OB90A64M1.h"
#include <stdint.h>

typedef void (*uart_callback_t)(uint8_t);

/*****************Macro****************/
//  Interrupt
#define UART_INT_RBR            (0x01 << 0x00)  //  Receiver Buffer Register Interrupt
#define UART_INT_THRE           (0x01 << 0x01)  //  Transmitter Holding Register Empty Interrupt
#define UART_INT_RXL            (0x01 << 0x02)  //  RX Line Interrupt
#define UART_INT_TEMT           (0x01 << 0x06)  //  UTHR and the UTSR are empty.
#define UART_INT_ABEO           (0x01 << 0x08)  //  End of Auto-Baud Interrupt
#define UART_INT_ABTO           (0x01 << 0x09)  //  Auto-Baud Time-Out Interrupt

#define UART_WORD_LEN_5         (0X00 << 0X00)
#define UART_WORD_LEN_6         (0X01 << 0X00)
#define UART_WORD_LEN_7         (0X02 << 0X00)
#define UART_WORD_LEN_8         (0X03 << 0X00)

#define UART_STOP_BIT_1         (0X00 << 0X03)
#define UART_STOP_BIT_2         (0X01 << 0X03)

#define UART_PARITY_NONE        (0X00 << 0X04)
#define UART_PARITY_ODD         (0X01 << 0X04)
#define UART_PARITY_EVEN        (0X03 << 0X04)
#define UART_PARITY_HIGH        (0X05 << 0X04)
#define UART_PARITY_LO          (0X07 << 0X04)

#define UART_FIFO_ENABLE        (0x01 << 0x00)
#define UART_FIFO_RESET_RX      (0x01 << 0x01)
#define UART_FIFO_RESET_TX      (0x01 << 0x02)

#define UART_FIFO_LEVEL_1BYTE   (0x00 << 0x06)
#define UART_FIFO_LEVEL_4BYTE   (0x01 << 0x06)
#define UART_FIFO_LEVEL_8BYTE   (0x02 << 0x06)
#define UART_FIFO_LEVEL_14BYTE  (0x03 << 0x06)

#define UART_STATUS_RDR         0x01
#define UART_STATUS_OE          0x02
#define UART_STATUS_PE          0x04
#define UART_STATUS_FE          0x08
#define UART_STATUS_BI          0x10
#define UART_STATUS_THRE        0x20
#define UART_STATUS_TEMT        0x40
#define UART_STATUS_RXFE        0x80


/*****************Enum*****************/


/****************Struct****************/
#pragma pack(1)


#pragma pack()


/***************Variable***************/


/***************Function***************/
void     UART_Open(OB_UART_Type* pUart, uint32_t nBaudRate, uart_callback_t callback);
void     UART_DisableInt(OB_UART_Type*  pUart, uint32_t nInterruptFlag);
void     UART_EnableInt(OB_UART_Type* pUart, uint32_t nInterruptFlag);
uint32_t UART_Write(OB_UART_Type* pUart, uint8_t *pTxBuf, uint32_t nWriteBytes);
uint32_t UART_Read(OB_UART_Type* pUart, uint8_t *pRxBuf, uint32_t nReadBytes);


/**************************************/

#endif
