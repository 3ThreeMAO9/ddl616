#ifndef __I2C_H
#define __I2C_H


uint32_t I2C_WaitINT(OB_I2C_Type *pI2C, uint32_t nTimeout);
void     I2C_ClaerINTFlag(OB_I2C_Type *pI2C, uint32_t nFlagType);

// For Master
void     I2C_MasterOpen(OB_I2C_Type *pI2c, uint32_t nBusClock);
void     I2C_MasterClose(OB_I2C_Type *pI2c);

uint8_t  I2C_MasterTransmit(OB_I2C_Type *pI2C, uint32_t nSlaveAddr, uint8_t* pData, uint32_t nDataLen);
uint8_t  I2C_MasterReceive(OB_I2C_Type *pI2C, uint32_t nSlaveAddr, uint8_t* pData, uint32_t nDataLen);

// For Slave
void     I2C_SlaveOpen(OB_I2C_Type *pI2c);
void     I2C_SlaveClose(OB_I2C_Type *pI2c);

void     I2C_SetSlaveAddr(OB_I2C_Type *pI2c, uint8_t nSlaveNo, uint8_t nSlaveAddr);
void     I2C_SetSlaveAddrMask(OB_I2C_Type *pI2c, uint8_t nSlaveNo, uint8_t nSlaveAddrMask);

uint8_t  I2C_SlaveTransmit(OB_I2C_Type* pI2C, uint8_t* pData, uint32_t nDataLen);
uint8_t  I2C_SlaveReceive(OB_I2C_Type* pI2C, uint8_t* pBuff, uint32_t* pnSize);


#endif

