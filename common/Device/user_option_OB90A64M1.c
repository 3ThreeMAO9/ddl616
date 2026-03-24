#include "stdint.h"
#include "chip_config.h"

/********************************************************************
User Options (Setting by User)
********************************************************************/
#define UOP_CODE_PROTECT    0x01        // Code protect.
                                        // 0 = Enabled
                                        // 1 = Disabled
                            
#define UOP_ISP_ENTRY       0x01        // ISP Entry.
                                        // 0x00   = CPU address start from user code. (APROM)
                                        // Others = CPU address start from ISP code.
                           
// IAP start address
#define UOP_IAP_STR_ADDR    FLASH_APP_BEGIN_ADDR

// IAP end address
#define UOP_IAP_END_ADDR    (FLASH_APP_BEGIN_ADDR + FLASH_APP_MAX_SIZE - 1)
                            
#define UOP_PADE_RESET      0x00        // Pad enable reset
                                        // 0x5A   = GPIO function mode.
                                        // Others = RESET function mode.

#define UOP_DATA_P0         0xFFFF      // GPIO output data.
#define UOP_DATA_P1         0xFFFF      // GPIO output data.
#define UOP_DATA_P2         0xFCFF      // GPIO output data.
#define UOP_DATA_P3         0xFFFF      // GPIO output data.
        
// GPIO Mode : Determine IO mode of the GPIO PortX_X.
// 0 = Pullup
// 1 = Open drain
// 2 = Push Pull
// 3 = Input Mode
#define UOP_MODE_P0_0       0U
#define UOP_MODE_P0_1       0U
#define UOP_MODE_P0_2       0U
#define UOP_MODE_P0_3       0U
#define UOP_MODE_P0_4       2U // spi mosi
#define UOP_MODE_P0_5       0U // spi miso
#define UOP_MODE_P0_6       0U
#define UOP_MODE_P0_7       0U
#define UOP_MODE_P0_8       2U // spi cs
#define UOP_MODE_P0_9       0U
#define UOP_MODE_P0_10      0U
#define UOP_MODE_P0_11      0U
#define UOP_MODE_P0_12      2U
#define UOP_MODE_P0_13      0U
#define UOP_MODE_P0_14      0U
#define UOP_MODE_P0_15      0U

#define UOP_MODE_P1_0       3U
#define UOP_MODE_P1_1       3U
#define UOP_MODE_P1_2       3U
#define UOP_MODE_P1_3       3U
#define UOP_MODE_P1_4       3U
#define UOP_MODE_P1_5       3U
#define UOP_MODE_P1_6       3U
#define UOP_MODE_P1_7       3U
#define UOP_MODE_P1_8       3U
#define UOP_MODE_P1_9       3U
#define UOP_MODE_P1_10      3U
#define UOP_MODE_P1_11      3U
#define UOP_MODE_P1_12      3U
#define UOP_MODE_P1_13      3U
#define UOP_MODE_P1_14      3U
#define UOP_MODE_P1_15      3U

#define UOP_MODE_P2_0       3U
#define UOP_MODE_P2_1       3U
#define UOP_MODE_P2_2       3U
#define UOP_MODE_P2_3       3U
#define UOP_MODE_P2_4       3U
#define UOP_MODE_P2_5       3U
#define UOP_MODE_P2_6       3U
#define UOP_MODE_P2_7       3U
#define UOP_MODE_P2_8       3U
#define UOP_MODE_P2_9       2U
#define UOP_MODE_P2_10      2U // spi clk
#define UOP_MODE_P2_11      3U
#define UOP_MODE_P2_12      3U
#define UOP_MODE_P2_13      3U
#define UOP_MODE_P2_14      3U
#define UOP_MODE_P2_15      3U

#define UOP_MODE_P3_0       3U
#define UOP_MODE_P3_1       3U
#define UOP_MODE_P3_2       3U
#define UOP_MODE_P3_3       3U
#define UOP_MODE_P3_4       3U
#define UOP_MODE_P3_5       3U
#define UOP_MODE_P3_6       3U
#define UOP_MODE_P3_7       3U
#define UOP_MODE_P3_8       3U
#define UOP_MODE_P3_9       3U
#define UOP_MODE_P3_10      3U
#define UOP_MODE_P3_11      3U
#define UOP_MODE_P3_12      3U
#define UOP_MODE_P3_13      3U
#define UOP_MODE_P3_14      3U
#define UOP_MODE_P3_15      3U

                         
/********************************************************************
Process and Allocate User Options Data (Setting by Vendor)
********************************************************************/

#define UOP_MODE_P0         ( (UOP_MODE_P0_15<<30) | (UOP_MODE_P0_14<<28) | (UOP_MODE_P0_13<<26) | (UOP_MODE_P0_12<<24) |  \
                              (UOP_MODE_P0_11<<22) | (UOP_MODE_P0_10<<20) | (UOP_MODE_P0_9 <<18) | (UOP_MODE_P0_8 <<16) |  \
                              (UOP_MODE_P0_7 <<14) | (UOP_MODE_P0_6 <<12) | (UOP_MODE_P0_5 <<10) | (UOP_MODE_P0_4 <<8 ) |  \
                              (UOP_MODE_P0_3 <<6 ) | (UOP_MODE_P0_2 <<4 ) | (UOP_MODE_P0_1 <<2 ) | (UOP_MODE_P0_0) )

#define UOP_MODE_P1         ( (UOP_MODE_P1_15<<30) | (UOP_MODE_P1_14<<28) | (UOP_MODE_P1_13<<26) | (UOP_MODE_P1_12<<24) |  \
                              (UOP_MODE_P1_11<<22) | (UOP_MODE_P1_10<<20) | (UOP_MODE_P1_9 <<18) | (UOP_MODE_P1_8 <<16) |  \
                              (UOP_MODE_P1_7 <<14) | (UOP_MODE_P1_6 <<12) | (UOP_MODE_P1_5 <<10) | (UOP_MODE_P1_4 <<8 ) |  \
                              (UOP_MODE_P1_3 <<6 ) | (UOP_MODE_P1_2 <<4 ) | (UOP_MODE_P1_1 <<2 ) | (UOP_MODE_P1_0) )

#define UOP_MODE_P2         ( (UOP_MODE_P2_15<<30) | (UOP_MODE_P2_14<<28) | (UOP_MODE_P2_13<<26) | (UOP_MODE_P2_12<<24) |  \
                              (UOP_MODE_P2_11<<22) | (UOP_MODE_P2_10<<20) | (UOP_MODE_P2_9 <<18) | (UOP_MODE_P2_8 <<16) |  \
                              (UOP_MODE_P2_7 <<14) | (UOP_MODE_P2_6 <<12) | (UOP_MODE_P2_5 <<10) | (UOP_MODE_P2_4 <<8 ) |  \
                              (UOP_MODE_P2_3 <<6 ) | (UOP_MODE_P2_2 <<4 ) | (UOP_MODE_P2_1 <<2 ) | (UOP_MODE_P2_0) )

#define UOP_MODE_P3         ( (UOP_MODE_P3_15<<30) | (UOP_MODE_P3_14<<28) | (UOP_MODE_P3_13<<26) | (UOP_MODE_P3_12<<24) |  \
                              (UOP_MODE_P3_11<<22) | (UOP_MODE_P3_10<<20) | (UOP_MODE_P3_9 <<18) | (UOP_MODE_P3_8 <<16) |  \
                              (UOP_MODE_P3_7 <<14) | (UOP_MODE_P3_6 <<12) | (UOP_MODE_P3_5 <<10) | (UOP_MODE_P3_4 <<8 ) |  \
                              (UOP_MODE_P3_3 <<6 ) | (UOP_MODE_P3_2 <<4 ) | (UOP_MODE_P3_1 <<2 ) | (UOP_MODE_P3_0) )

#if defined (__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050)
    #pragma clang diagnostic push
    #pragma clang diagnostic ignored "-Wmissing-variable-declarations"
#endif

const uint32_t rMMCRP       __attribute__((section(".ARM.__at_0x0000FE00"), used)) = UOP_CODE_PROTECT;
const uint32_t rISPET       __attribute__((section(".ARM.__at_0x0000FE04"), used)) = UOP_ISP_ENTRY;
const uint32_t rIAPSA       __attribute__((section(".ARM.__at_0x0000FE08"), used)) = UOP_IAP_STR_ADDR;
const uint32_t rIAPEA       __attribute__((section(".ARM.__at_0x0000FE0C"), used)) = UOP_IAP_END_ADDR;
const uint32_t rCTRO        __attribute__((section(".ARM.__at_0x0000FE10"), used)) = (UOP_PADE_RESET << 16);
const uint32_t rGPIODATA0   __attribute__((section(".ARM.__at_0x0000FE20"), used)) = UOP_DATA_P0;
const uint32_t rGPIOMODE0   __attribute__((section(".ARM.__at_0x0000FE24"), used)) = UOP_MODE_P0;
const uint32_t rGPIODATA1   __attribute__((section(".ARM.__at_0x0000FE28"), used)) = UOP_DATA_P1;
const uint32_t rGPIOMODE1   __attribute__((section(".ARM.__at_0x0000FE2C"), used)) = UOP_MODE_P1;
const uint32_t rGPIODATA2   __attribute__((section(".ARM.__at_0x0000FE30"), used)) = UOP_DATA_P2;
const uint32_t rGPIOMODE2   __attribute__((section(".ARM.__at_0x0000FE34"), used)) = UOP_MODE_P2;
const uint32_t rGPIODATA3   __attribute__((section(".ARM.__at_0x0000FE38"), used)) = UOP_DATA_P3;
const uint32_t rGPIOMODE3   __attribute__((section(".ARM.__at_0x0000FE3C"), used)) = UOP_MODE_P3;

#if defined (__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050)
    #pragma clang diagnostic pop
#endif
