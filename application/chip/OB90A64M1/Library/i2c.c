#include "OB90A64M1.h"
#include "i2c.h"

#define I2C_INTFLAG_SIPULS  0x02
#define I2C_INTFLAG_ACK     0x04
#define I2C_INTFLAG_SI      0x08
#define I2C_INTFLAG_STO     0x10
#define I2C_INTFLAG_STA     0x20

volatile uint32_t g_nINT0Status = 0;
volatile uint8_t  g_nINT0Flag   = 0;
volatile uint32_t g_nINT1Status = 0;
volatile uint8_t  g_nINT1Flag   = 0;

// Private Function
uint32_t I2C_GetPeripheralClock(OB_I2C_Type *pI2C);
void     I2C_Delay(uint32_t nDelay);


void I2C0_IRQHandler(void)
{
    g_nINT0Status = OB_I2C0->STAT;
    OB_I2C0->CONCLR = I2C_INTFLAG_SIPULS ;
    g_nINT0Flag = 0x01;
}

void I2C1_IRQHandler(void)
{
    g_nINT1Status = OB_I2C1->STAT;
    OB_I2C1->CONCLR = I2C_INTFLAG_SIPULS;
    g_nINT1Flag = 0x01;
}

void I2C_MasterOpen(OB_I2C_Type *pI2C, uint32_t nBusClock)
{
    uint32_t nNum;

    // Setting I2C BusClock
    nNum = I2C_GetPeripheralClock(pI2C) / nBusClock / 2;
    pI2C->SCLL = nNum;
    pI2C->SCLH = nNum;

    // Clear Flags
    if (pI2C == OB_I2C0)
        g_nINT0Flag = 0x00;
    else
        g_nINT1Flag = 0x00;

    pI2C->CONCLR = 0xFFFFFFFF;

    // Enable I2C
    pI2C->CONSET_b.I2EN = 1;

    // Enable I2C IRQ
    if (pI2C == OB_I2C0)
        NVIC_EnableIRQ(I2C0_IRQn);
    else
        NVIC_EnableIRQ(I2C1_IRQn);
}

void I2C_MasterClose(OB_I2C_Type *pI2C)
{
    // Disable I2C
    pI2C->CONCLR_b.I2ENC = 1;

    // Disable I2C IRQ
    if (pI2C == OB_I2C0)
        NVIC_DisableIRQ(I2C0_IRQn);
    else
        NVIC_DisableIRQ(I2C1_IRQn);
}

void I2C_ClaerINTFlag(OB_I2C_Type *pI2C, uint32_t nFlagType)
{
    if (pI2C == OB_I2C0)
        g_nINT0Flag = 0x00;
    else
        g_nINT1Flag = 0x00;

    pI2C->CONCLR = nFlagType;
}

uint32_t I2C_WaitINT(OB_I2C_Type *pI2C, uint32_t nTimeout)
{
    if (pI2C == OB_I2C0)
    {
        while(nTimeout--)
        {
            if (g_nINT0Flag)
            {
                return g_nINT0Status;
            }
        }
    }
    else
    {
        while(nTimeout--)
        {
            if (g_nINT1Flag)
            {
                return g_nINT1Status;
            }
        }
    }

    return 0x00; // Timeout
}

uint8_t I2C_MasterTransmit(OB_I2C_Type *pI2C, uint32_t nSlaveAddr, uint8_t* pData, uint32_t nDataLen)
{
    uint32_t nLoop = 0;

    // I2C Start
    pI2C->CONSET_b.STA = 1;
    if (I2C_WaitINT(pI2C, 0xFFFFFFFF) != 0x08)
        return FALSE;
    I2C_ClaerINTFlag(pI2C, I2C_INTFLAG_STA);

    // Transmit Slave Addr and WriteBit
    pI2C->DAT = ((nSlaveAddr<<1) | 0x00); // Addr | (Write bit)
    I2C_ClaerINTFlag(pI2C, I2C_INTFLAG_SI);
    if (I2C_WaitINT(pI2C, 0xFFFFFFFF) != 0x18)
        return FALSE;
    I2C_ClaerINTFlag(pI2C, I2C_INTFLAG_ACK);

    // Loop for Transmit Data
    for (nLoop = 0x00; nLoop < nDataLen; nLoop++)
    {
        I2C_Delay(10);

        // Transmit Data
        pI2C->DAT = (*pData);
        I2C_ClaerINTFlag(pI2C, I2C_INTFLAG_SI);

        // Wait Ack
        if (I2C_WaitINT(pI2C, 0xFFFFFFFF) != 0x28)
            return FALSE;

        pData++;
    }
    // I2C Stop
    pI2C->CONSET_b.STO = 1;
    I2C_ClaerINTFlag(pI2C, I2C_INTFLAG_SI);
    pI2C->CONCLR = 0X10;
    return TRUE;
}

uint8_t I2C_MasterReceive(OB_I2C_Type *pI2C, uint32_t nSlaveAddr, uint8_t* pData, uint32_t nDataLen)
{
    uint32_t nLoop = 0;

    // Start I2C
    pI2C->CONSET_b.STA = 1;
    if (I2C_WaitINT(pI2C, 0xFFFFFFFF) != 0x08)
        return FALSE;
    I2C_ClaerINTFlag(pI2C, I2C_INTFLAG_STA);

    // Transmit Slave Addr and ReadBit
    pI2C->DAT = ((nSlaveAddr<<1) | 0x01);  // Addr | (Read bit = 0x01)
    I2C_ClaerINTFlag(pI2C, I2C_INTFLAG_SI);
    if (I2C_WaitINT(pI2C, 0xFFFFFFFF) != 0x40)
        return FALSE;
    I2C_ClaerINTFlag(pI2C, I2C_INTFLAG_ACK);

    // Loop for Receive Data
    for (nLoop = 0x00; nLoop < nDataLen; nLoop++)
    {
        I2C_Delay(10);

        if (nLoop == (nDataLen-1))
        {
            pI2C->CONCLR_b.AAC = 1;  // Set NACK
            I2C_ClaerINTFlag(pI2C, I2C_INTFLAG_SI);
            if (I2C_WaitINT(pI2C, 0xFFFFFFFF) != 0x58)
                return FALSE;
        }
        else
        {
            pI2C->CONSET_b.AA = 1;  // Set ACK
            I2C_ClaerINTFlag(pI2C, I2C_INTFLAG_SI);
            if (I2C_WaitINT(pI2C, 0xFFFFFFFF) != 0x50)
                return FALSE;
        }

        *pData = pI2C->DAT;
        pData++;
    }

    // I2C Stop
    pI2C->CONSET_b.STO = 1;
    I2C_ClaerINTFlag(pI2C, I2C_INTFLAG_SI);
    pI2C->CONCLR = 0X10;
    return TRUE;
}

void I2C_SetSlaveAddr(OB_I2C_Type *pI2C, uint8_t nSlaveNo, uint8_t nSlaveAddr)
{
    switch (nSlaveNo)
    {
    case 0:
        pI2C->ADR0  = (nSlaveAddr << 1);
        break;
    case 1:
        pI2C->ADR1  = (nSlaveAddr << 1);
        break;
    }
}

void I2C_SetSlaveAddrMask(OB_I2C_Type *pI2C, uint8_t nSlaveNo, uint8_t nSlaveAddrMask)
{
    switch (nSlaveNo)
    {
    case 0:
        pI2C->MASK0 = nSlaveAddrMask << 1;
        break;
    case 1:
        pI2C->MASK1 = nSlaveAddrMask << 1;
        break;
    }
}

void I2C_SlaveOpen(OB_I2C_Type *pI2C)
{
    // Clear Flags
    if (pI2C == OB_I2C0)
        g_nINT0Flag = 0x00;
    else
        g_nINT1Flag = 0x00;

    pI2C->CONCLR = 0xFFFFFFFF;

    // Enable I2C
    pI2C->CONSET_b.I2EN = 1;
    pI2C->CONSET_b.AA   = 1;

    // Enable I2C IRQ
    if (pI2C == OB_I2C0)
        NVIC_EnableIRQ(I2C0_IRQn);
    else
        NVIC_EnableIRQ(I2C1_IRQn);
}

void I2C_SlaveClose(OB_I2C_Type *pI2C)
{
    // Disable I2C
    pI2C->CONCLR_b.I2ENC = 1;

    // Disable I2C IRQ
    if (pI2C == OB_I2C0)
        NVIC_DisableIRQ(I2C0_IRQn);
    else
        NVIC_DisableIRQ(I2C1_IRQn);
}

uint8_t I2C_SlaveReceive(OB_I2C_Type *pI2C, uint8_t* pBuff, uint32_t* pnSize)
{
    uint32_t nLoop = 0;
    uint32_t nINTStatus = 0;
    //----------------------- chip addr
    if (I2C_WaitINT(pI2C, 0xFFFFFFFF) != 0x60)
        return FALSE;

    I2C_ClaerINTFlag(pI2C, I2C_INTFLAG_SI);
    //-------------------------------------------------------
    for (nLoop = 0; nLoop <= (*pnSize); nLoop++)
    {
        nINTStatus = I2C_WaitINT(pI2C, 0xFFFFFFFF);

        if (nINTStatus == 0x80) // Data Received
        {
            *pBuff = pI2C->DAT;
            I2C_ClaerINTFlag(pI2C, I2C_INTFLAG_SI);
            pBuff++;
        }
        else if (nINTStatus == 0xA0) // Stop Condition
        {
            (*pnSize) = nLoop;
            I2C_ClaerINTFlag(pI2C, I2C_INTFLAG_SI);
            return TRUE;
        }
        else
        {
            (*pnSize) = nLoop;
            I2C_ClaerINTFlag(pI2C, I2C_INTFLAG_SI);
            return FALSE;
        }
    }

    (*pnSize) = nLoop;
    return FALSE;
}


uint8_t I2C_SlaveTransmit(OB_I2C_Type* pI2C, uint8_t* pData, uint32_t nDataLen)
{
    uint32_t nLoop = 0;
    uint32_t nINTStatus = 0;

    //----------------------- chip addr
    if (I2C_WaitINT(pI2C, 0xFFFFFFFF) != 0xA8)
        return FALSE;

    I2C_ClaerINTFlag(pI2C, I2C_INTFLAG_SI);

    for (nLoop = 0x00; nLoop <= nDataLen; nLoop++)
    {
        pI2C->DAT = (*pData);

        nINTStatus = I2C_WaitINT(pI2C, 0xFFFFFFFF);

        if (nINTStatus == 0xB8)
        {
            pData++;
            I2C_ClaerINTFlag(pI2C, I2C_INTFLAG_SI);
        }
        else if (nINTStatus == 0xC0) // End Transmit
        {
            I2C_ClaerINTFlag(pI2C, I2C_INTFLAG_SI);
            return TRUE;
        }
        else
        {
            I2C_ClaerINTFlag(pI2C, I2C_INTFLAG_SI);
            return FALSE;
        }
    }

    return FALSE;
}

uint32_t I2C_GetINTStatus(OB_I2C_Type *pI2C)
{
    if (pI2C == OB_I2C0)
    {
        if (g_nINT0Flag)
        {
            return g_nINT0Status;
        }
    }
    else
    {
        if (g_nINT1Flag)
        {
            return g_nINT1Status;
        }
    }

    return 0x00;
}

uint32_t I2C_GetPeripheralClock(OB_I2C_Type *pI2C)
{
    uint32_t Div = 0;

    if (pI2C == OB_I2C0)
    {
        Div = (1<<OB_SYSCON->PCLKSEL0_b.CS_I2C0);
    }
    else
    {
        Div = (1<<OB_SYSCON->PCLKSEL0_b.CS_I2C1);
    }

    return (SystemCoreClock / Div);
}

void I2C_Delay(uint32_t nDelay)
{
    while(nDelay--);
}


