#include <stdint.h>
#include "OB90A64M1.h"
#include "system_OB90A64M1.h"
#include "math.h"

// System Clock Type
#define SYS_CLK_TYPE_HIRC       0x00
#define SYS_CLK_TYPE_XTAL       0x01
#define SYS_CLK_TYPE_HIRC_DPLL  0x02
#define SYS_CLK_TYPE_XTAL_DPLL  0x03

// DPLL Clock Option
#define DPLL_CLK_12M        12000000
#define DPLL_CLK_16M        16000000
#define DPLL_CLK_32M        32000000
#define DPLL_CLK_48M        48000000
#define DPLL_CLK_50M        50000000
#define DPLL_CLK_64M        64000000

// System Clock (Setting by User)
#define DEFAULT_SYS_CLOCK_TYPE  SYS_CLK_TYPE_HIRC_DPLL       // Select system clock type

#define HIRC_FREQ           24000000                    // The HIRC frequence
#define XTAL_FREQ           24000000                    // The XTAL frequence
#define DPLL_FREQ           DPLL_CLK_64M                // The DPLL frequence


#if (DEFAULT_SYS_CLOCK_TYPE == SYS_CLK_TYPE_HIRC )
    #define DEFAULT_SYS_CLOCK HIRC_FREQ
#elif (DEFAULT_SYS_CLOCK_TYPE == SYS_CLK_TYPE_XTAL)
    #define DEFAULT_SYS_CLOCK XTAL_FREQ
#else
    #define DEFAULT_SYS_CLOCK DPLL_FREQ
#endif

volatile uint32_t SystemCoreClock = DEFAULT_SYS_CLOCK; // Current system core clock.

void SystemInit (void)
{
    // Setting system clock

#if (DEFAULT_SYS_CLOCK_TYPE == SYS_CLK_TYPE_HIRC)
    OB_SYSCON->CCLKCFG = OB_SYSCON->CCLKCFG & 0xFFFFF0FF;
    while((OB_SYSCON->CPUCHIPCTR0 & 0x00800000) != 0x00800000);     // Wait Ready
#elif (DEFAULT_SYS_CLOCK_TYPE == SYS_CLK_TYPE_XTAL)
    uint32_t* pCPUCHIPCTR0;
    uint32_t* pCPUCHIPCTR1;
    pCPUCHIPCTR0 = (uint32_t*) 0x500100B0;
    pCPUCHIPCTR1 = (uint32_t*) 0x500100B4;
    //--------------------Enable XTAL
    *pCPUCHIPCTR0 = *pCPUCHIPCTR0 | 0x08;                           //Enable XTAL
    //-------------filter
    if(XTAL_FREQ <= 30000000)
        *pCPUCHIPCTR1 = (*pCPUCHIPCTR1 & 0xFFFFFFF3) | 0x04;
    else
        *pCPUCHIPCTR1 = (*pCPUCHIPCTR1 & 0xFFFFFFF3) | 0x08;
    //--------------------force HIRC
    OB_SYSCON->CCLKCFG = OB_SYSCON->CCLKCFG & 0xFFFFF0FF;
    while((OB_SYSCON->CPUCHIPCTR0 & 0x00800000) != 0x00800000);     // Wait Ready

    OB_SYSCON->CCLKCFG = OB_SYSCON->CCLKCFG & 0xFFFFF0FF;
    OB_SYSCON->CCLKCFG = OB_SYSCON->CCLKCFG | 0x0200;
    while((OB_SYSCON->CPUCHIPCTR0 & 0x00200000) != 0x00200000);     // Wait Ready

#else
    // DPLL Init
    uint32_t* pCPUCHIPCTR0;
    uint32_t* pCPUCHIPCTR1;
    uint32_t nInputSounre = 0;
    uint32_t nDiv_2M = 0;
    volatile uint32_t* pDPLLCT;
    volatile uint32_t* pDPLLDM;
    //--------------------------------------
    pDPLLDM = (uint32_t*) 0x50010090;
    pDPLLCT = (uint32_t*) 0x50010098;
    *pDPLLDM = 0x02;
    *pDPLLCT = (183) | (188 <<16);
    //------------------------------------------
    pCPUCHIPCTR0 = (uint32_t*) 0x500100B0;
    pCPUCHIPCTR1 = (uint32_t*) 0x500100B4;
    //--------------------force HIRC
    OB_SYSCON->CCLKCFG = OB_SYSCON->CCLKCFG & 0xFFFFF0FF;
    while((OB_SYSCON->CPUCHIPCTR0 & 0x00800000) != 0x00800000);     // Wait Ready

    if (DEFAULT_SYS_CLOCK_TYPE == SYS_CLK_TYPE_HIRC_DPLL)
    {
        nInputSounre = 0x01;
        nDiv_2M = (HIRC_FREQ / 2000000) - 1;
        nDiv_2M = (nDiv_2M << 0x04);
    }
    else
    {
        *pCPUCHIPCTR0 = *pCPUCHIPCTR0 | 0x08;  //Enable XTAL
        //-------------filter
        if(XTAL_FREQ <= 30000000)
            *pCPUCHIPCTR1 = (*pCPUCHIPCTR1 & 0xFFFFFFF3) | 0x04;
        else
            *pCPUCHIPCTR1 = (*pCPUCHIPCTR1 & 0xFFFFFFF3) | 0x08;
         //-----------------------------------------------------------------

        nDiv_2M = (XTAL_FREQ / 2000000) - 1;
        nDiv_2M = (nDiv_2M << 0x04);
    }

    switch (DPLL_FREQ)
    {
    case DPLL_CLK_12M:
        OB_SYSCON->DPLLCON = (0xF0081000) | ( nDiv_2M) | nInputSounre; // 192/16 = 12M
        break;
    case DPLL_CLK_16M:
        OB_SYSCON->DPLLCON = (0xF0080C00) | ( nDiv_2M) | nInputSounre; // 192/12 = 16M
        break;
    case DPLL_CLK_32M:
        OB_SYSCON->DPLLCON = (0xF0080600) | ( nDiv_2M) | nInputSounre; // 192/6 = 32M
        break;
    case DPLL_CLK_48M:
        OB_SYSCON->DPLLCON = (0xF0080400) | ( nDiv_2M) | nInputSounre; // 192/4 = 48M
        break;
    case DPLL_CLK_50M:
        *pDPLLCT = (191) | (196 <<16);
        OB_SYSCON->DPLLCON = (0xF0090400) | ( nDiv_2M) | nInputSounre; // 200/4 = 50M
        break;
    case DPLL_CLK_64M:
        OB_SYSCON->DPLLCON = (0xF0080300) | ( nDiv_2M) | nInputSounre; // 192/3 = 64M
        break;
    default:
        OB_SYSCON->DPLLCON = (0xF0080600) | ( nDiv_2M) | nInputSounre; // 192/6 = 32M
        break;
    }

    if (DEFAULT_SYS_CLOCK > 24000000)
    {
        OB_SYSCON->CCLKCFG = OB_SYSCON->CCLKCFG | 0x0400; // CPU Wait 1 Cycle
        OB_SYSCON->CCLKCFG = OB_SYSCON->CCLKCFG | 0x0100;
    }
    else
    {
        OB_SYSCON->CCLKCFG = OB_SYSCON->CCLKCFG | 0x0100;
    }

    while((OB_SYSCON->CPUCHIPCTR0 & 0x00400000) != 0x00400000);     // Wait Ready
#endif
}

void SystemCoreClockUpdate (void)
{
    uint32_t nDiv = OB_SYSCON->CCLKCFG & 0x00FF;

    SystemCoreClock = DEFAULT_SYS_CLOCK / (nDiv + 1);
}

uint32_t GetPeripheralClock(uint32_t nAPBModuleIndex)
{
    uint32_t nDiv, nBuff;

    if ((nAPBModuleIndex == APB_T32B0) || (nAPBModuleIndex == APB_T32B1))
        nBuff = OB_SYSCON->PCLKSEL1 >> nAPBModuleIndex;
    else
        nBuff = OB_SYSCON->PCLKSEL0 >> nAPBModuleIndex;

    if (nBuff == 0x00)
        nDiv = 0x01;
    else if (nBuff == 0x01)
        nDiv = 0x02;
    else if (nBuff == 0x02)
        nDiv = 0x04;
    else
        nDiv = 0x08;

    return (SystemCoreClock / nDiv);
}

void WaitingCPUReady(void)
{
#if (DEFAULT_SYS_CLOCK_TYPE == SYS_CLK_TYPE_HIRC)
    while((OB_SYSCON->CPUCHIPCTR0 & 0x00800000) != 0x00800000);     // Wait Ready
#elif (DEFAULT_SYS_CLOCK_TYPE == SYS_CLK_TYPE_XTAL)
    while((OB_SYSCON->CPUCHIPCTR0 & 0x00200000) != 0x00200000);     // Wait Ready
#else
    while((OB_SYSCON->CPUCHIPCTR0 & 0x00400000) != 0x00400000);     // Wait Ready
#endif
}
