#include <stdio.h>
#include "OB90A64M1.h"
#include "dma.h"
#include "gpio.h"

volatile uint32_t gDMA_Int = 0x00;
void PDMA_IRQHandler(void)
{
    if( (OB_PDMA0->PDMA_CSR & DMA_INTERRUPT_STATUS) != 0x00)
    {
        OB_PDMA0->PDMA_CSR = OB_PDMA0->PDMA_CSR | DMA_INTERRUPT_STATUS; // clear status
        gDMA_Int = gDMA_Int | 0x01;
    }
    if( (OB_PDMA1->PDMA_CSR & DMA_INTERRUPT_STATUS) != 0x00)
    {
        OB_PDMA1->PDMA_CSR = OB_PDMA1->PDMA_CSR | DMA_INTERRUPT_STATUS; // clear status
        gDMA_Int = gDMA_Int | 0x02;
    }
    if( (OB_PDMA2->PDMA_CSR & DMA_INTERRUPT_STATUS) != 0x00)
    {
        OB_PDMA2->PDMA_CSR = OB_PDMA2->PDMA_CSR | DMA_INTERRUPT_STATUS; // clear status
        gDMA_Int = gDMA_Int | 0x04;
    }
    if( (OB_PDMA3->PDMA_CSR & DMA_INTERRUPT_STATUS) != 0x00)
    {
        OB_PDMA3->PDMA_CSR = OB_PDMA3->PDMA_CSR | DMA_INTERRUPT_STATUS; // clear status
        gDMA_Int = gDMA_Int | 0x08;
    }
}

void DMA_Enable(OB_PDMA_Type *pDMA)
{
    pDMA->PDMA_CSR = pDMA->PDMA_CSR | 0x01;

}

void DMA_InterruptEnable(OB_PDMA_Type *pDMA)
{
    pDMA->PDMA_CSR = pDMA->PDMA_CSR | DMA_INTERRUPT_ENABLE;

}

void DMA_DataLength(OB_PDMA_Type *pDMA, uint32_t Length)
{
    pDMA->PDMA_CYC = Length;

}

//-------------------------------- 從SFR抓資料到RAM
void DMA_Open1(OB_PDMA_Type *pDMA,uint32_t SFR_Addr, uint32_t RAM_Addr, uint32_t DataLength )
{
    pDMA->PDMA_CSR = 0x00;
    pDMA->PDMA_CSR =  pDMA->PDMA_CSR | (DMA_BUS_APB << 6) | (DMA_BUS_AHB<< 7 );


    DMA_SourceAddr(pDMA, SFR_Addr);      // 讀SFR的位置
    DMA_SourceMode(pDMA,DMA_MODE_INC_0);   // 讀固定位置, 所以位置加 4
    DMA_SourecEvent(pDMA, DMA_EVENT_ADC0);  //TRIG

    DMA_DestinationAddr(pDMA,RAM_Addr );  // 放到RAM的位置
    DMA_DestinationMode(pDMA,DMA_MODE_INC_4);// 放完後RAM的位置加4 (word)
    DMA_DestinationEvent(pDMA, 0);

    DMA_DataWidth(pDMA, DMA_WIDTH_WORD);// 存取的單位
    DMA_DataLength(pDMA, DataLength);
    DMA_InterruptEnable(pDMA);
}
//-------------------------------- 抓資料放入SFR 中

void DMA_Open2(OB_PDMA_Type *pDMA,uint32_t SFR_Addr, uint32_t RAM_Addr, uint32_t DataLength)
{
    pDMA->PDMA_CSR = 0x00;
    pDMA->PDMA_CSR =  pDMA->PDMA_CSR | (DMA_BUS_AHB << 6) | (DMA_BUS_APB<< 7 );

    DMA_SourceAddr(pDMA, RAM_Addr);       // 讀RAM的位置
    DMA_SourceMode(pDMA,DMA_MODE_INC_4);     // 讀固定位置, 所以位置 + 4
    DMA_SourecEvent(pDMA, 0);

    DMA_DestinationAddr(pDMA,SFR_Addr );    // 放到RAM的位置
    DMA_DestinationMode(pDMA,DMA_MODE_INC_4); // 放固定位置, 所以位置 + 4
    DMA_DestinationEvent(pDMA, 0);

    DMA_DataWidth(pDMA, DMA_WIDTH_WORD);// 存取的單位
    DMA_DataLength(pDMA, DataLength);
    DMA_InterruptEnable(pDMA);
}


//-------------------------------- 抓資料放入SFR 中 ( 使用 EVENT)
void DMA_Open3(OB_PDMA_Type *pDMA,uint32_t SFR_Addr, uint32_t RAM_Addr, uint32_t Event, uint32_t DataLength)
{
    pDMA->PDMA_CSR = 0x00;
    pDMA->PDMA_CSR =  pDMA->PDMA_CSR | (DMA_BUS_AHB << 6) | (DMA_BUS_APB<< 7 );

    DMA_SourceAddr(pDMA, RAM_Addr);        // 讀RAM的位置
    DMA_SourceMode(pDMA,DMA_MODE_INC_4);     // 讀 後位置加 4
    DMA_SourecEvent(pDMA, Event);

    DMA_DestinationAddr(pDMA,SFR_Addr );    // 放到SFR的位置
    DMA_DestinationMode(pDMA,DMA_MODE_INC_0); // 放固定位置, 所以位置不變
    DMA_DestinationEvent(pDMA, 0);

    DMA_DataWidth(pDMA, DMA_WIDTH_WORD);// 存取的單位
    DMA_DataLength(pDMA, DataLength);
    DMA_InterruptEnable(pDMA);
}

//-------------------------------- 從SFR抓資料到RAM UART1
void DMA_Open4(OB_PDMA_Type *pDMA,uint32_t SourceBus, uint32_t DesBus, uint32_t Event, uint32_t DataLength )
{
    pDMA->PDMA_CSR = 0x00;
    pDMA->PDMA_CSR =  pDMA->PDMA_CSR | (SourceBus << 6) | (DesBus<< 7 );


    DMA_SourceAddr(pDMA, 0x40030000);      // 讀SFR的位置
    DMA_SourceMode(pDMA,DMA_MODE_INC_0);   // 讀固定位置, 所以位置不變
    DMA_SourecEvent(pDMA, Event);

    DMA_DestinationAddr(pDMA,RAM_ADDR_RX );  // 放到RAM的位置
    DMA_DestinationMode(pDMA,DMA_MODE_INC_4);// 放完後RAM的位置加4 (word)
    DMA_DestinationEvent(pDMA, 0);

    DMA_DataWidth(pDMA, DMA_WIDTH_WORD);// 存取的單位
    DMA_DataLength(pDMA, DataLength);
    DMA_InterruptEnable(pDMA);
}

//-------------------------------- 從memory抓資料到memory
void DMA_Open5(OB_PDMA_Type *pDMA,uint32_t Sourec_Addr, uint32_t Des_Addr, uint32_t DataLength)
{
    pDMA->PDMA_CSR = 0x00;
    pDMA->PDMA_CSR =  pDMA->PDMA_CSR | (DMA_BUS_AHB << 6) | (DMA_BUS_AHB<< 7 );

    DMA_SourceAddr(pDMA, Sourec_Addr);        // 讀RAM的位置
    DMA_SourceMode(pDMA,DMA_MODE_INC_4);     // 讀 後位置加 4


    DMA_DestinationAddr(pDMA,Des_Addr );    // 放到RAM的位置
    DMA_DestinationMode(pDMA,DMA_MODE_INC_4); // 放固定位置, 所以位置不變

    DMA_DataWidth(pDMA, DMA_WIDTH_WORD);// 存取的單位
    DMA_DataLength(pDMA, DataLength);
    DMA_InterruptEnable(pDMA);
}

void DMA_DataWidth(OB_PDMA_Type *pDMA,uint32_t Width )
{

    pDMA->PDMA_CSR = (pDMA->PDMA_CSR & 0xFF0FFFFF) | (Width << 20);
}
//---------------------------------------Source----------------------------------------------------
void DMA_SourceAddr(OB_PDMA_Type *pDMA, uint32_t SourceAddr)
{
    pDMA->PDMA_SRCADR = SourceAddr;

}


void DMA_SourceMode(OB_PDMA_Type *pDMA,uint32_t AddrMode)
{

    pDMA->PDMA_CSR = (pDMA->PDMA_CSR & 0xFFFFF0FF) | (AddrMode << 8);
}


void DMA_SourecEvent(OB_PDMA_Type *pDMA, uint32_t SourecEvent)
{
    pDMA->PDMA_CSR = (pDMA->PDMA_CSR & 0xF0FFFFFF) | (SourecEvent << 24);
}

//-------------------------------------------------------------------------------------------


void DMA_DestinationAddr(OB_PDMA_Type *pDMA, uint32_t DestinationAddr)
{

    pDMA->PDMA_DESADR = DestinationAddr;
}




void DMA_DestinationMode(OB_PDMA_Type *pDMA,uint32_t AddrMode)
{

    pDMA->PDMA_CSR = (pDMA->PDMA_CSR & 0xFFFF0FFF) | (AddrMode << 12);
}


void DMA_DestinationEvent(OB_PDMA_Type *pDMA, uint32_t DestinationEvent)
{

    pDMA->PDMA_CSR = (pDMA->PDMA_CSR & 0xFFF0FFFF) | (DestinationEvent << 16);
}
