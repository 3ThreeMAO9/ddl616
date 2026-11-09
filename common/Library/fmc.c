#include "OB90A64M1.h"
#include "fmc.h"
#include <string.h>

#define OB_LOG_LEVEL OB_LOG_LEVEL_DEFAULT
#include "ob_log.h"
#define TAG "fmc"

void FMC_Close()
{
    OB_FMC->ISPCC = 0xE8000000;
}

/**
  * @retval    0   Success
  * @retval   -1   Erase failed
  */
int32_t FMC_ChipErase()
{
    uint32_t nLoop;
    OB_FMC->ISPCC  = FMC_ISP_CHIP_ERASE | 0xE8000001;
    OB_FMC->ISPADR = 0x00;
    OB_FMC->ISPTS  = 0xE8000001;
    nLoop = 0xFFFFFF;
    while(nLoop--)
    {
        if ((OB_FMC->ISPTS & 0x01) == 0x00)
            return 0x00;
    }
    return -1;
}

/**
  * @retval       0         Success
  * @retval      -1         Erase failed
  */
int32_t FMC_PageErase(uint32_t nPageAddr)
{
    uint32_t nLoop;

    OB_FMC->ISPCC  = FMC_ISP_PAGE_ERASE | 0xE8000001;
    OB_FMC->ISPADR = nPageAddr;
    OB_FMC->ISPTS  = 0xE8000001;
    nLoop = 0xFFFFFF;
    while(nLoop--)
    {
        if ((OB_FMC->ISPTS & 0x01) == 0x00)
            return 0x00;
    }
    return -1;
}

uint32_t FMC_GetBootSource()
{
    if ((OB_FMC->BSCSR & 0x02) == 0x00)
        return 0;
    else
        return 1;
}

void FMC_SetBootTo(uint32_t target)
{
    OB_FMC->BSCSR_b.BS = (target == FMC_BOOT_TO_OTA) ? 0x02 : 0x00;
}

void FMC_Read(uint32_t nAddr, uint8_t *pData, uint32_t nLength)
{
    /*
    uint32_t nLoop;
    OB_FMC->ISPADR = nAddr;
    OB_FMC->ISPCC  = FMC_ISP_READ | 0xE8000001;
    nLoop = 0x00;
    while(1)
    {
        if (nLoop >= nLength)
            break;

        OB_FMC->ISPTS = 0xE8000001;
        if (FMC_WaitFinish() ==  FALSE)
            return;

        *((uint32_t*)pData) = OB_FMC->ISPDAT;
        pData += 4;
        nLoop += 4;
    }
    */
    /* 原读取以4字节对齐,如添加的结构体不是4字节对齐, 就会越界读, 因此修改 */

    memcpy(pData, (void *)nAddr, nLength);
}

uint32_t FMC_Read_Boot(uint32_t addr, uint32_t *buf, uint32_t size)
{
    OB_FMC->ISPCC = FMC_ISP_READ | 0xE8000001;
    OB_FMC->ISPADR = addr;

    while (size)
    {
        OB_FMC->ISPTS = 0xE8000001;
        if (FMC_WaitFinish() ==  0)
            return 0;

        *buf = OB_FMC->ISPDAT;
        buf++;
        size--;
    }

    return 1;
}

// uint32_t FMC_ReadCompanyID(uint32_t nPos)
// {
//     uint32_t nID;
//     uint8_t  RBuff[4];

//     if (nPos == 0x00)
//         FMC_Read(0x00100000, 4, RBuff);
//     else
//         FMC_Read(0x00100004, 4, RBuff);

//     nID = *((uint32_t*)RBuff);
//     return nID;
// }


void FMC_Write(uint32_t nAddr, uint8_t *pData, uint32_t nLength)
{
    uint32_t nLoop;
    uint32_t tmp;
    OB_FMC->ISPCC  = FMC_ISP_PROGRAM | 0xE8000001;
    OB_FMC->ISPADR = nAddr;

    nLoop = 0x00;

    while(1)
    {
        if (nLoop>= nLength)
            break;
		tmp=0;
		tmp+=(uint32_t)(*pData++);////kaung, 注意必须是字节模式。
		tmp+=((uint32_t)(*pData++)<<8);
		tmp+=((uint32_t)(*pData++)<<16);
		tmp+=((uint32_t)(*pData++)<<24);

		
        OB_FMC->ISPDAT = tmp;// *((uint32_t *)pData);

       // OB_FMC->ISPDAT = *((uint32_t *)pData);
        OB_FMC->ISPTS  = 0xE8000001;
	
        if (FMC_WaitFinish() ==  0)
            return;

       // pData += 4;
        nLoop += 4;
    }
}

uint32_t FMC_WaitFinish(void)
{
    uint32_t nLoop;

    nLoop = 0xFFFFFF;
    while(nLoop--)
    {
        if ((OB_FMC->ISPTS & 0x01) == 0x00)
            return 1;
    }
    return 0;
}

int32_t FMC_ReadConfig(uint32_t *pConfig, uint32_t nStartAddr, uint32_t nCount)
{
    uint32_t nLoop;
    OB_FMC->ISPADR = nStartAddr;

    while(1)
    {
        if (nCount == 0x00)
            return 0x00;

        OB_FMC->ISPCC = FMC_ISP_READ | 0xE8000001;
        OB_FMC->ISPTS = 0xE8000001;
        nLoop = 0xFFFFFF;
        while(nLoop--)
        {
            if ((OB_FMC->ISPTS & 0x01) == 0x00)
            {
                *pConfig = OB_FMC->ISPDAT;
                break;
            }
            if (nLoop == 0x01)
            {
                return -1;
            }
        }

        pConfig = pConfig + 4;
        nCount--;
    }
}

int32_t FMC_WriteConfig(uint32_t *pConfig, uint32_t nStartAddr, uint32_t nCount)
{
    uint32_t nLoop;
    OB_FMC->ISPADR = nStartAddr;

    while(1)
    {
        if (nCount == 0x00)
            return 0x00;

        OB_FMC->ISPCC  = FMC_ISP_PROGRAM | 0xE8000001;
        OB_FMC->ISPDAT = *pConfig;
        OB_FMC->ISPTS  = 0xE8000001;
        nLoop = 0xFFFFFF;
        while(nLoop--)
        {
            if ((OB_FMC->ISPTS & 0x01) == 0x00)
                break;
            if (nLoop == 0x01)
                return -1;
        }
        pConfig = pConfig + 4;
        nCount  = nCount - 1;
    }
}

void FMC_ReLoad(void)
{
    OB_FMC->ISPCC  = FMC_ISP_CHIP_RELOAD | 0xE8000001;
    OB_FMC->ISPADR = 0x00;
    OB_FMC->ISPTS  = 0xE8000001;
    FMC_WaitFinish();
}
