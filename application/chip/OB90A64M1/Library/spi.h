#ifndef __SPI_H
#define __SPI_H

#define SPI_SR_TFE  (1 << 0)  // 发送FIFO为空
#define SPI_SR_TNF  (1 << 1)  // 发送FIFO未满
#define SPI_SR_RNE  (1 << 2)  // 接收FIFO非空
#define SPI_SR_RFF  (1 << 3)  // 接收FIFO满
#define SPI_SR_BSY  (1 << 4)  // SPI忙

#define SPI_SIZE_4BIT           (0x03)
#define SPI_SIZE_5BIT           (0x04)
#define SPI_SIZE_6BIT           (0x05)
#define SPI_SIZE_7BIT           (0x06)
#define SPI_SIZE_8BIT           (0x07)
#define SPI_SIZE_9BIT           (0x08)
#define SPI_SIZE_10BIT          (0x09)
#define SPI_SIZE_11BIT          (0x0A)
#define SPI_SIZE_12BIT          (0x0B)
#define SPI_SIZE_13BIT          (0x0C)
#define SPI_SIZE_14BIT          (0x0D)
#define SPI_SIZE_15BIT          (0x0E)
#define SPI_SIZE_16BIT          (0x0F)

#define SPI_FORMAT_SPI          (0 << 0x04)
#define SPI_FORMAT_TI           (1 << 0x04)
#define SPI_FORMAT_MICROWIRE    (2 << 0x04)

#define SPI_OUT_POLARITY_HI     (0 << 0x06)
#define SPI_OUT_POLARITY_LO     (1 << 0x06)

#define SPI_OUT_PHASE_FIRST     (0 << 0x07)
#define SPI_OUT_PHASE_SECOND    (1 << 0x07)

#define SPI_INT_RORIM       (0x01 << 0x00)
#define SPI_INT_RTIM        (0x01 << 0x01)
#define SPI_INT_RXIM        (0x01 << 0x02)
#define SPI_INT_TXIM        (0x01 << 0x03)

#define SPI_RX_SiZE  20

extern volatile uint32_t g_SPIRxCounter;
extern volatile uint8_t g_SPIRxData[SPI_RX_SiZE];


uint32_t SPI_MasterOpen(OB_SPI_Type *pSPI, uint32_t nSPICapability, uint32_t nBusClock);
void     SPI_Close(OB_SPI_Type *pSPI);
void     SPI_EnableINT(OB_SPI_Type *pSPI, uint32_t nINTSel);
void     SPI_DisableINT(OB_SPI_Type *pSPI, uint32_t nINTSel);
void     SPI_ClearRxFIFO(OB_SPI_Type *pSPI);
uint8_t  SPI_ClearTxFIFO(OB_SPI_Type *pSPI);
uint32_t SPI_SetBusClock(OB_SPI_Type *pSPI, uint32_t BusClock);
uint32_t SPI_GetBusClock(OB_SPI_Type *pSPI);

uint32_t SPI_WriteFIFO(OB_SPI_Type *pSPI, const uint8_t *pBuf, uint32_t len);
uint32_t SPI_ReadWithClock(OB_SPI_Type *pSPI, uint8_t *pBuf, uint32_t len);

void spi_init(void);
void spi_sleep_init(void);

#endif
