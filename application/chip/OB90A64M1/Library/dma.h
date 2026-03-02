#ifndef __DMA_H
#define __DMA_H

void DMA_Open1(OB_PDMA_Type *pDMA,uint32_t SFR_ADD, uint32_t RAM_ADD, uint32_t DataLength );
void DMA_Open2(OB_PDMA_Type *pDMA,uint32_t SFR_Addr, uint32_t RAM_Addr, uint32_t DataLength);
void DMA_Open3(OB_PDMA_Type *pDMA,uint32_t SFR_Addr, uint32_t RAM_Addr,uint32_t Event, uint32_t DataLength);
void DMA_Open4(OB_PDMA_Type *pDMA,uint32_t SourceBus, uint32_t DesBus, uint32_t Event, uint32_t DataLength );
void DMA_Open5(OB_PDMA_Type *pDMA,uint32_t Sourec_Addr, uint32_t Des_Addr, uint32_t DataLength);
void DMA_DataWidth(OB_PDMA_Type *pDMA,uint32_t Width );
void DMA_SourceAddr(OB_PDMA_Type *pDMA, uint32_t SourceAddr);
void DMA_SourceMode(OB_PDMA_Type *pDMA,uint32_t AddrMode);
void DMA_SourecEvent(OB_PDMA_Type *pDMA, uint32_t SourecEvent);


void DMA_DestinationAddr(OB_PDMA_Type *pDMA, uint32_t DestinationAddr);
void DMA_DestinationMode(OB_PDMA_Type *pDMA,uint32_t AddrMode);
void DMA_DestinationEvent(OB_PDMA_Type *pDMA, uint32_t DestinationEvent);
void DMA_Enable(OB_PDMA_Type *pDMA);
void DMA_DataLength(OB_PDMA_Type *pDMA, uint32_t Length);
#define DMA_EVENT_SPI_Tx   0x01
#define DMA_EVENT_SPI_Rx   0x02
#define DMA_EVENT_UART0_Tx 0x03
#define DMA_EVENT_UART0_Rx 0x04
#define DMA_EVENT_IIC_Tx   0x05
#define DMA_EVENT_IIC_Rx   0x06
#define DMA_EVENT_UART1_Tx 0x07
#define DMA_EVENT_UART1_Rx 0x08
#define DMA_EVENT_ADC0     0x09
#define DMA_EVENT_ADC1     0x0A
#define DMA_EVENT_ps11_gnt 0x0B
#define DMA_EVENT_ps12_gnt 0x0C
#define DMA_EVENT_ps13_gnt 0x0D
#define DMA_EVENT_ps14_gnt 0x0E
#define DMA_EVENT_ps15_gnt 0x0F

#define DMA_MODE_INC_0  0x00
#define DMA_MODE_INC_1  0x01
#define DMA_MODE_INC_2  0x02
#define DMA_MODE_INC_4  0x03


#define DMA_BUS_APB 0x00
#define DMA_BUS_AHB 0x01

#define DMA_WIDTH_WORD      0x00
#define DMA_WIDTH_HALF_WORD 0x01
#define DMA_WIDTH_BYTE      0x02

#define DMA_INTERRUPT_ENABLE      0x04
#define DMA_INTERRUPT_STATUS      0x02

#define RAM_ADDR_RX 0x20001800
#define RAM_ADDR_TX 0x20000EF0

extern volatile uint32_t gDMA_Int;

#endif
