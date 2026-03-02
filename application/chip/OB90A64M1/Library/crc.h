#ifndef __CRC_H
#define __CRC_H

#define CRC_POLY_CRCCCITT   0x00            /** CRC CCITT polynomial */
#define CRC_POLY_CRC8       0x01            /** CRC-8 polynomial */
#define CRC_POLY_CRC16      0x02            /** CRC-16 polynomial */
#define CRC_POLY_CRC32      0x03            /** CRC-32 polynomial */

#define CRC_WDL_32  0x00
#define CRC_WDL_16  0x01
#define CRC_WDL_8   0x02


void     CRC_Open(uint32_t nPolyMode, uint32_t nLengthMode, uint32_t nSeed);
void     CRC_Close(void);

uint32_t CRC_Calculate(uint8_t* pData, uint32_t nDataLength);




#endif
