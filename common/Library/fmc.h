#ifndef __FMC_H 
#define __FMC_H

int32_t  FMC_ChipErase(void);
int32_t  FMC_PageErase(uint32_t nPageAddr);
void     FMC_Read(uint32_t nAddr, uint8_t *pData, uint32_t nLength);
void     FMC_Write(uint32_t nAddr, uint8_t *pData, uint32_t nLength);
uint32_t FMC_ReadCompanyID(uint32_t nPos);
uint32_t FMC_GetBootSource(void);
void     FMC_SetBootTo(uint32_t target);
int32_t  FMC_ReadConfig(uint32_t *pConfig, uint32_t nStartAddr, uint32_t nCount);
int32_t  FMC_WriteConfig(uint32_t *pConfig, uint32_t nStartAddr, uint32_t nCount);
uint32_t FMC_WaitFinish(void);
void     FMC_ReLoad(void);

#define FMC_ISP_READ        ( 0x01  << 0x04)
#define FMC_ISP_PROGRAM     ( 0x02  << 0x04)
#define FMC_ISP_PAGE_ERASE  ( 0x04  << 0x04)
#define FMC_ISP_CHIP_ERASE  ( 0x08  << 0x04)
#define FMC_ISP_CHIP_RELOAD ( 0x0F  << 0x04)

#define FMC_BOOT_TO_APP (0)
#define FMC_BOOT_TO_OTA (1)

#endif
