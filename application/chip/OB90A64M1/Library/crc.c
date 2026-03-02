#include "OB90A64M1.h"
#include "crc.h"

void CRC_Open(uint32_t nPolyMode, uint32_t nLengthMode, uint32_t nSeed)
{
    OB_CRC->CTL_b.EN    = 1;
    OB_CRC->CTL_b.MODE  = nPolyMode;
    OB_CRC->CTL_b.CRWDLEN = nLengthMode;
    OB_CRC->SEED        = nSeed;

    // Reload Seed value to circuit.
    OB_CRC->CTL_b.RST   = 1;
}

void CRC_Close()
{
    OB_CRC->CTL = 0x00000000;
}

uint32_t CRC_Calculate(uint8_t *pData, uint32_t nDataLength)
{
    uint8_t  nBuff8;
    uint16_t nBuff16;
    uint32_t nBuff32;
    uint32_t nLoop = 0x00;
    while(1)
    {
        if (nLoop >= nDataLength)
            break;

        if (OB_CRC->CTL_b.CRWDLEN == CRC_WDL_32)
        {
            nBuff32    = (uint32_t)*pData;
            OB_CRC->WD = (uint32_t)nBuff32;
            pData += 4;
            nLoop += 4;
        }
        else if (OB_CRC->CTL_b.CRWDLEN == CRC_WDL_16)
        {
            nBuff16    = (uint16_t)*pData;
            OB_CRC->WD = (uint32_t)nBuff16;
            pData += 2;
            nLoop += 2;
        }
        else if (OB_CRC->CTL_b.CRWDLEN == CRC_WDL_8)
        {
            nBuff8     = (uint8_t)*pData;
            OB_CRC->WD = (uint32_t)nBuff8;
            pData++;
            nLoop++;
        }
    }
    return OB_CRC->CS;
}
