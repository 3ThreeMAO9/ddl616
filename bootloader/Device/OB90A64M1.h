
/****************************************************************************************************//**
 * @file     OB90A64M1.h
 *
 * @brief    CMSIS Cortex-M0 Peripheral Access Layer Header File for
 *           OB90A64M1 from On-Bright.
 *
 * @version  V0.1
 * @date     10. May 2018
 *
 * @note     Generated with SVDConv V2.75 
 *           from CMSIS SVD File 'OB90A64M1.svd' Version 0.1,
 *******************************************************************************************************/



/** @addtogroup On-Bright
  * @{
  */

/** @addtogroup OB90A64M1
  * @{
  */

#ifndef OB90A64M1_H
#define OB90A64M1_H

#ifdef __cplusplus
extern "C" {
#endif


/* -------------------------  Interrupt Number Definition  ------------------------ */

typedef enum {
/* -------------------  Cortex-M0 Processor Exceptions Numbers  ------------------- */
  Reset_IRQn                    = -15,              /*!<   1  Reset Vector, invoked on Power up and warm reset                 */
  NonMaskableInt_IRQn           = -14,              /*!<   2  Non maskable Interrupt, cannot be stopped or preempted           */
  HardFault_IRQn                = -13,              /*!<   3  Hard Fault, all classes of Fault                                 */
  SVCall_IRQn                   =  -5,              /*!<  11  System Service Call via SVC instruction                          */
  DebugMonitor_IRQn             =  -4,              /*!<  12  Debug Monitor                                                    */
  PendSV_IRQn                   =  -2,              /*!<  14  Pendable request for system service                              */
  SysTick_IRQn                  =  -1,              /*!<  15  System Tick Timer                                                */
/* --------------------  OB90A64M1 Specific Interrupt Numbers  -------------------- */
  RESERVED0_IRQn                =   0,              /*!<   0  Reserved interrupt                                               */
  I2C1_IRQn                     =   1,              /*!<   1  I2C1 interrupt                                                   */
  LVI_IRQn                      =   2,              /*!<   2  LVI interrupt                                                    */
  ACMP_IRQn                     =   3,              /*!<   3  ACMP interrupt                                                   */
  RTC_IRQn                      =   4,              /*!<   4  RTC interrupt                                                    */
  I2C0_IRQn                     =   5,              /*!<   5  I2C0 interrupt                                                   */
  CT16B0_IRQn                   =   6,              /*!<   6  CT16B0 interrupt                                                 */
  CT16B1_IRQn                   =   7,              /*!<   7  CT16B1 interrupt                                                 */
  CT32B0_IRQn                   =   8,              /*!<   8  CT32B0 interrupt                                                 */
  CT32B1_IRQn                   =   9,              /*!<   9  CT32B1 interrupt                                                 */
  SPI_IRQn                      =  10,              /*!<  10  SPI interrupt                                                    */
  UART0_IRQn                    =  11,              /*!<  11  UART0 interrupt                                                  */
  QEI_IRQn                      =  12,              /*!<  12  QEI interrupt                                                    */
  PDMA_IRQn                     =  13,              /*!<  13  PDMA interrupt                                                   */
  PWM_IRQn                      =  14,              /*!<  14  PWM interrupt                                                    */
  WDT_IRQn                      =  15,              /*!<  15  WDT interrupt                                                    */
  ADC_IRQn                      =  16,              /*!<  16  ADC interrupt                                                    */
  UART1_IRQn                    =  17,              /*!<  17  UART1 interrupt                                                  */
  GPIO0_IRQn                    =  18,              /*!<  18  GPIO0 interrupt                                                  */
  GPIO1_IRQn                    =  19,              /*!<  19  GPIO1 interrupt                                                  */
  GPIO2_IRQn                    =  20,              /*!<  20  GPIO2 interrupt                                                  */
  GPIO3_IRQn                    =  21,              /*!<  21  GPIO3 interrupt                                                  */
  PWMTM_IRQn                    =  22,              /*!<  22  PWMTM interrupt                                                  */
  RESERVED1_IRQn                =  23,              /*!<  23  Reserved interrupt                                               */
  RESERVED2_IRQn                =  24,              /*!<  24  Reserved interrupt                                               */
  RESERVED3_IRQn                =  25,              /*!<  25  Reserved interrupt                                               */
  RESERVED4_IRQn                =  26,              /*!<  26  Reserved interrupt                                               */
  RESERVED5_IRQn                =  27,              /*!<  27  Reserved interrupt                                               */
  RESERVED6_IRQn                =  28,              /*!<  28  Reserved interrupt                                               */
  RESERVED7_IRQn                =  29,              /*!<  29  Reserved interrupt                                               */
  RESERVED8_IRQn                =  30,              /*!<  30  Reserved interrupt                                               */
  RESERVED9_IRQn                =  31               /*!<  31  Reserved interrupt                                               */
} IRQn_Type;


/** @addtogroup Configuration_of_CMSIS
  * @{
  */


/* ================================================================================ */
/* ================      Processor and Core Peripheral Section     ================ */
/* ================================================================================ */

/* ----------------Configuration of the Cortex-M0 Processor and Core Peripherals---------------- */
#define __CM0_REV                 0x0000            /*!< Cortex-M0 Core Revision                                               */
#define __MPU_PRESENT                  0            /*!< MPU present or not                                                    */
#define __NVIC_PRIO_BITS               2            /*!< Number of Bits used for Priority Levels                               */
#define __Vendor_SysTickConfig         0            /*!< Set to 1 if different SysTick Config is used                          */
/** @} */ /* End of group Configuration_of_CMSIS */

#include <core_cm0.h>                               /*!< Cortex-M0 processor and core peripherals                              */
#include "system_OB90A64M1.h"                       /*!< OB90A64M1 System                                                      */


/* ================================================================================ */
/* ================       Device Specific Peripheral Section       ================ */
/* ================================================================================ */


/** @addtogroup Device_Peripheral_Registers
  * @{
  */


/* -------------------  Start of section using anonymous unions  ------------------ */
#if defined(__CC_ARM)
  #pragma push
  #pragma anon_unions
#elif defined(__ICCARM__)
  #pragma language=extended
#elif defined(__GNUC__)
  /* anonymous unions are enabled by default */
#elif defined(__TMS470__)
/* anonymous unions are enabled by default */
#elif defined(__TASKING__)
  #pragma warning 586
#else
  #warning Not supported compiler type
#endif



/* ================================================================================ */
/* ================                       I2C                      ================ */
/* ================================================================================ */


/**
  * @brief I2C-bus controller (I2C)
  */

typedef struct {                                    /*!< I2C Structure                                                         */
  
  union {
    __IO uint32_t  CONSET;                          /*!< Control Set Register. When a one is written to a bit of this
                                                         register, the corresponding bit in the I2C control register
                                                          is set. Writing a zero has no effect on the corresponding bit
                                                          in the I2C control register.                                         */
    
    struct {
           uint32_t             :  1;
      __IO uint32_t  SIPULSE    :  1;               /*!< I2C interrupt flag. (function same bit 3 (SI) , connect CPU
                                                         interrupt service)                                                    */
      __IO uint32_t  AA         :  1;               /*!< Assert acknowledge flag.                                              */
      __IO uint32_t  SI         :  1;               /*!< I2C interrupt flag.                                                   */
      __IO uint32_t  STO        :  1;               /*!< STOP flag.                                                            */
      __IO uint32_t  STA        :  1;               /*!< START flag.                                                           */
      __IO uint32_t  I2EN       :  1;               /*!< I2C interface enable.                                                 */
    } CONSET_b;                                     /*!< BitSize                                                               */
  };
  
  union {
    __I  uint32_t  STAT;                            /*!< Status Register. During I2C operation, this register provides
                                                         detailed status codes that allow software to determine the next
                                                          action needed.                                                       */
    
    struct {
           uint32_t             :  3;
      __I  uint32_t  STATUS     :  5;               /*!< These bits give the actual status information about the I2C
                                                         interface.                                                            */
    } STAT_b;                                       /*!< BitSize                                                               */
  };
  
  union {
    __IO uint32_t  DAT;                             /*!< Data Register. During master or slave transmit mode, data to
                                                         be transmitted is written to this register. During master or
                                                          slave receive mode, data that has been received may be read
                                                          from this register.                                                  */
    
    struct {
      __IO uint32_t  DATA       :  8;               /*!< This register holds data values that have been received or are
                                                         to be transmitted.                                                    */
    } DAT_b;                                        /*!< BitSize                                                               */
  };
  
  union {
    __IO uint32_t  ADR0;                            /*!< Slave Address Register 0. Contains the 7-bit slave address for
                                                         operation of the I2C interface in slave mode, and is not used
                                                          in master mode. The least significant bit determines whether
                                                          a slave responds to the General Call address.                        */
    
    struct {
      __IO uint32_t  GC         :  1;               /*!< General Call enable bit.                                              */
      __IO uint32_t  ADDR       :  7;               /*!< The I2C device address for slave mode.                                */
    } ADR0_b;                                       /*!< BitSize                                                               */
  };
  
  union {
    __IO uint32_t  SCLH;                            /*!< SCH Duty Cycle Register High Half Word. Determines the high
                                                         time of the I2C clock.                                                */
    
    struct {
      __IO uint32_t  SCLH       : 16;               /*!< Count for SCL HIGH time period selection.                             */
    } SCLH_b;                                       /*!< BitSize                                                               */
  };
  
  union {
    __IO uint32_t  SCLL;                            /*!< SCL Duty Cycle Register Low Half Word. Determines the low time
                                                         of the I2C clock. I2nSCLL and I2nSCLH together determine the
                                                          clock frequency generated by an I2C master and certain times
                                                          used in slave mode.                                                  */
    
    struct {
      __IO uint32_t  SCLL       : 16;               /*!< Count for SCL low time period selection.                              */
    } SCLL_b;                                       /*!< BitSize                                                               */
  };
  
  union {
    __O  uint32_t  CONCLR;                          /*!< Control Clear Register. When a one is written to a bit of this
                                                         register, the corresponding bit in the control register is cleared.
                                                          Writing a zero has no effect on the corresponding bit in the
                                                          control register.                                                    */
    
    struct {
           uint32_t             :  2;
      __O  uint32_t  AAC        :  1;               /*!< Assert acknowledge Clear bit.                                         */
      __O  uint32_t  SIC        :  1;               /*!< I2C interrupt Clear bit.                                              */
           uint32_t             :  1;
      __O  uint32_t  STAC       :  1;               /*!< START flag Clear bit.                                                 */
      __O  uint32_t  I2ENC      :  1;               /*!< I2C interface Disable bit.                                            */
    } CONCLR_b;                                     /*!< BitSize                                                               */
  };
  __I  uint32_t  RESERVED0;
  
  union {
    __IO uint32_t  ADR1;                            /*!< Slave Address Register 1. Contains the 7-bit slave address for
                                                         operation of the I2C interface in slave mode, and is not used
                                                          in master mode. The least significant bit determines whether
                                                          a slave responds to the General Call address.                        */
    
    struct {
      __IO uint32_t  GC         :  1;               /*!< General Call enable bit.                                              */
      __IO uint32_t  ADDR       :  7;               /*!< The I2C device address for slave mode.                                */
    } ADR1_b;                                       /*!< BitSize                                                               */
  };
  __I  uint32_t  RESERVED1[2];
  
  union {
    __I  uint32_t  DATABUFFER;                      /*!< Data buffer register. The contents of the 8 MSBs of the I2DAT
                                                         shift register will be transferred to the DATA_BUFFER automatically
                                                          after every nine bits (8 bits of data plus ACK or NACK) has
                                                          been received on the bus.                                            */
    
    struct {
      __I  uint32_t  DATA       :  8;               /*!< This register holds contents of the 8 MSBs of the DAT shift
                                                         register.                                                             */
    } DATABUFFER_b;                                 /*!< BitSize                                                               */
  };
  
  union {
    __IO uint32_t  MASK0;                           /*!< Slave address mask register. This mask register is associated
                                                         with I2ADR0 to determine an address match. The mask register
                                                          has no effect when comparing to the General Call address ('0000000'). */
    
    struct {
           uint32_t             :  1;
      __IO uint32_t  MASK       :  7;               /*!< Mask bits.                                                            */
    } MASK0_b;                                      /*!< BitSize                                                               */
  };
  
  union {
    __IO uint32_t  MASK1;                           /*!< Slave address mask register. This mask register is associated
                                                         with I2ADR0 to determine an address match. The mask register
                                                          has no effect when comparing to the General Call address ('0000000'). */
    
    struct {
           uint32_t             :  1;
      __IO uint32_t  MASK       :  7;               /*!< Mask bits.                                                            */
    } MASK1_b;                                      /*!< BitSize                                                               */
  };
  
  union {
    __I  uint32_t  RDDAT;                           /*!< Read Data register. DMA function enable.                              */
    
    struct {
      __I  uint32_t  RDATA      :  8;               /*!< This register holds contents of the 8 MSBs of the DAT shift
                                                         register.                                                             */
    } RDDAT_b;                                      /*!< BitSize                                                               */
  };
  
  union {
    __IO uint32_t  CONDMA;                          /*!< Control DMA register 1. DMA function control.                         */
    
    struct {
      __IO uint32_t  DMACNT     : 28;               /*!< .                                                                     */
      __IO uint32_t  DMARREQ    :  1;               /*!< DMA Receiver Request. (Read only)                                     */
      __IO uint32_t  DMATREQ    :  1;               /*!< DMA Transmitter Request. (Read only)                                  */
      __IO uint32_t  ACK_NACK   :  1;               /*!< .                                                                     */
      __IO uint32_t  DMAE       :  1;               /*!< DMA Enable                                                            */
    } CONDMA_b;                                     /*!< BitSize                                                               */
  };
} OB_I2C_Type;


/* ================================================================================ */
/* ================                      WWDT                      ================ */
/* ================================================================================ */


/**
  * @brief Windowed Watchdog Timer (WWDT) (WWDT)
  */

typedef struct {                                    /*!< WWDT Structure                                                        */
  
  union {
    __O  uint32_t  WWDTRLD;                         /*!< Window Watchdog Timer Reload Counter Register.                        */
    
    struct {
      __O  uint32_t  WWDTRLD    : 32;               /*!< Writing 0X00005AA5 to this register will reload the window watchdog
                                                         Timer counter value to 0x3F. Note: Software can only write WWDTRLD
                                                          to reload WWDT counter value when current WWDT counter value
                                                          between 0 and WINCMP. If software writes WWDTRLD when current
                                                          WWDT counter value is large than WINCMP, WWDT reset signal will
                                                          generate immediately.                                                */
    } WWDTRLD_b;                                    /*!< BitSize                                                               */
  };
  
  union {
    __IO uint32_t  WWDTCR;                          /*!< Window Watchdog Timer Control Register.                               */
    
    struct {
      __IO uint32_t  WWDTEN     :  1;               /*!< WWDT Enable. Set this bit to enable Window Watchdog Timer counter
                                                         counting.                                                             */
      __IO uint32_t  WWDTIE     :  1;               /*!< WWDT Interrupt Enable. Set this bit to enable the Window Watchdog
                                                         Timer time-out interrupt function.                                    */
           uint32_t             :  6;
      __IO uint32_t  PERIODSEL  :  4;               /*!< WWDT Prescale Period Select. These 4-bit select the prescale
                                                         period for the WWDT counter period.                                   */
           uint32_t             :  4;
      __IO uint32_t  WINCMP     :  6;               /*!< Set this register to adjust the valid reload window. Note: Software
                                                         can only write WWDTRLD to reload WWDT counter value when current
                                                          WWDT counter WWDT counter value between 0 and WINCMP. If Software
                                                          writes WWDTRLD when current WWDT counter value large than WICMP,
                                                          WWDT reset signal will generate immediately.                         */
           uint32_t             :  9;
      __IO uint32_t  DBGACK_WWDT:  1;               /*!< ICE debug mode acknowledge Disable. Set this bit to enable the
                                                         Window Watchdog Timer time-out interrupt function.                    */
    } WWDTCR_b;                                     /*!< BitSize                                                               */
  };
  
  union {
    __IO uint32_t  WWDTSR;                          /*!< Window Watchdog Timer Status Register.                                */
    
    struct {
      __IO uint32_t  WWDTIF     :  1;               /*!< WWDT Compare Match Interrupt Flag. When current WWDT counter
                                                         value matches WWCMP, this bit is set to 1. This bit will be
                                                          cleared by writing 1 to itself.                                      */
      __IO uint32_t  WWDTRF     :  1;               /*!< WWDT Reset Flag. When WWDT counter counts down to 0 or writes
                                                         WWDTRLD during current WWDT counter value being larger than
                                                          WINCMP, chip will be reset and this bit is set to 1. This bit
                                                          will be cleared to 0 by writing 1 to itself.                         */
    } WWDTSR_b;                                     /*!< BitSize                                                               */
  };
  
  union {
    __I  uint32_t  WWDTCVR;                         /*!< Window Watchdog Timer Counter Value Register.                         */
    
    struct {
      __I  uint32_t  WWDTCVAL   :  6;               /*!< WWDT Counter Value. This register reflects the current WWDT
                                                         counter value and is read only.                                       */
    } WWDTCVR_b;                                    /*!< BitSize                                                               */
  };
} OB_WWDT_Type;


/* ================================================================================ */
/* ================                      UART                      ================ */
/* ================================================================================ */


/**
  * @brief UART (UART)
  */

typedef struct {                                    /*!< UART Structure                                                        */
  
  union {
    union {
      __O  uint32_t  UTHR;                          /*!< Transmit Holding Register. The next character to be transmitted
                                                         is written here.                                                      */
      
      struct {
        __O  uint32_t  THR      :  9;               /*!< Writing to the USART Transmit Holding Register causes the data
                                                         to be stored in the USART transmit FIFO. The byte will be sent
                                                          when it is the oldest byte in the FIFO and the transmitter is
                                                          available.                                                           */
      } UTHR_b;                                     /*!< BitSize                                                               */
    };
    
    union {
      __IO uint32_t  URBR;                          /*!< Receiver Buffer Register. Contains the next received character
                                                         to be read.                                                           */
      
      struct {
        __IO uint32_t  RBR      :  9;               /*!< The UART Receiver Buffer Register contains the oldest received
                                                         byte in the UART RX FIFO.                                             */
      } URBR_b;                                     /*!< BitSize                                                               */
    };
  } ;
  __I  uint32_t  RESERVED0;
  
  union {
    __IO uint32_t  UDL;                             /*!< The UART Divisor Latch is part of the UART Baud Rate Generator
                                                         and holds the value used, along with the Fractional Divider,
                                                          to divide the UART_PCLK clock in order to produce the baud rate
                                                          clock, which must be 16x the desired baud rate. The UDL registers
                                                          together form a 16-bit divisor A 0x0000 value is treated like
                                                          a 0x0001 value as division by zero is not allowed.                   */
    
    struct {
      __IO uint32_t  UDL        : 16;               /*!< The UART Divisor Latch Register, determines the baud rate of
                                                         the UART.                                                             */
    } UDL_b;                                        /*!< BitSize                                                               */
  };
  
  union {
    __IO uint32_t  UIER;                            /*!< Interrupt Enable Register. Contains individual interrupt enable
                                                         bits for the 7 potential UART interrupts.                             */
    
    struct {
      __IO uint32_t  RBRIE      :  1;               /*!< URBR Interrupt Enable. Enables the Receive Data Available interrupt
                                                         for UART. It also controls the Character Receive Time-out interrupt.  */
      __IO uint32_t  THREIE     :  1;               /*!< THRE Interrupt Enable. Enables the THRE interrupt for UART.
                                                         The status of this interrupt can be read from ULSR[5].                */
      __IO uint32_t  RXLIE      :  1;               /*!< RX Line Interrupt Enable. Enables the UART RX line status interrupts.
                                                         The status of this interrupt can be read from ULSR[4:1].              */
           uint32_t             :  5;
      __IO uint32_t  ABEOINTEN  :  1;               /*!< Enables the end of auto-baud interrupt.                               */
      __IO uint32_t  ABTOINTEN  :  1;               /*!< Enables the auto-baud time-out interrupt.                             */
    } UIER_b;                                       /*!< BitSize                                                               */
  };
  
  union {
    __I  uint32_t  UIIR;                            /*!< Interrupt ID Register. Identifies which interrupt(s) are pending.     */
    
    struct {
      __I  uint32_t  INTSTATUS  :  1;               /*!< Interrupt status. Note that UIIR[0] is active low. The pending
                                                         interrupt can be determined by evaluating UIIR[3:1].                  */
      __I  uint32_t  INTID      :  3;               /*!< Interrupt identification. UIER[3:1] identifies an interrupt
                                                         corresponding to the UART Rx FIFO. All other combinations of
                                                          UIER[3:1] not listed below are reserved (100,101,111).               */
           uint32_t             :  2;
      __I  uint32_t  FIFOENABLE :  2;               /*!< These bits are equivalent to UFCR[0].                                 */
      __I  uint32_t  ABEOINT    :  1;               /*!< End of auto-baud interrupt. True if auto-baud has finished successfully
                                                         and interrupt is enabled.                                             */
      __I  uint32_t  ABTOINT    :  1;               /*!< Auto-baud time-out interrupt. True if auto-baud has timed out
                                                         and interrupt is enabled.                                             */
    } UIIR_b;                                       /*!< BitSize                                                               */
  };
  
  union {
    __O  uint32_t  UFCR;                            /*!< FIFO Control Register. Controls UART FIFO usage and modes.            */
    
    struct {
      __O  uint32_t  FIFOEN     :  1;               /*!< FIFO enable                                                           */
      __O  uint32_t  RXFIFORES  :  1;               /*!< RX FIFO Reset                                                         */
      __O  uint32_t  TXFIFORES  :  1;               /*!< TX FIFO Reset                                                         */
           uint32_t             :  3;
      __O  uint32_t  RXTL       :  2;               /*!< RX Trigger Level. These two bits determine how many receiver
                                                         UART FIFO characters must be written before an interrupt is
                                                          activated.                                                           */
    } UFCR_b;                                       /*!< BitSize                                                               */
  };
  
  union {
    __I  uint32_t  UFRC;                            /*!< The UFRC is the number of the UART RX FIFO                            */
    
    struct {
      __I  uint32_t  UFRC       :  4;               /*!< The UFRC is the number of the UART RX FIFO                            */
    } UFRC_b;                                       /*!< BitSize                                                               */
  };
  
  union {
    __I  uint32_t  UFTC;                            /*!< The UFTC is the number of the UART TX FIFO.                           */
    
    struct {
      __I  uint32_t  UFTC       :  4;               /*!< The UFTC is the number of the UART TX FIFO.                           */
    } UFTC_b;                                       /*!< BitSize                                                               */
  };
  
  union {
    __IO uint32_t  ULCR;                            /*!< Line Control Register. Contains controls for frame formatting
                                                         and break generation.                                                 */
    
    struct {
      __IO uint32_t  WLS        :  3;               /*!< Word Length Selection                                                 */
      __IO uint32_t  SBS        :  1;               /*!< Stop Bit Select                                                       */
      __IO uint32_t  PE         :  1;               /*!< Parity Enable                                                         */
      __IO uint32_t  PS         :  2;               /*!< Parity Select                                                         */
    } ULCR_b;                                       /*!< BitSize                                                               */
  };
  
  union {
    __I  uint32_t  ULSR;                            /*!< Line Status Register. Contains flags for transmit and receive
                                                         status, including line errors.                                        */
    
    struct {
      __I  uint32_t  RDR        :  1;               /*!< Receiver Data Ready. ULSR[0] is set when the URBR holds an unread
                                                         character and is cleared when the UART URBR FIFO is empty.            */
      __I  uint32_t  OE         :  1;               /*!< Overrun Error. The overrun error condition is set as soon as
                                                         it occurs. A ULSR read clears ULSR[1]. ULSR[1] is set when UART
                                                          RSR has a new character assembled and the UART RBR FIFO is full.
                                                          In this case, the UART RBR FIFO will not be overwritten and
                                                          the character in the UART RSR will be lost.                          */
      __I  uint32_t  PE         :  1;               /*!< Parity Error. When the parity bit of a received character is
                                                         in the wrong state, a parity error occurs. A ULSR read clears
                                                          ULSR[2]. Time of parity error detection is dependent on UFCR[0].
                                                          Note: A parity error is associated with the character at the
                                                          top of the UART RBR FIFO.                                            */
      __I  uint32_t  FE         :  1;               /*!< Framing Error. When the stop bit of a received character is
                                                         a logic 0, a framing error occurs. A ULSR read clears ULSR[3].
                                                          The time of the framing error detection is dependent on UFCR[0].
                                                          Upon detection of a framing error, the RX will attempt to re-synchronize
                                                          to the data and assume that the bad stop bit is actually an
                                                          early start bit. However, it cannot be assumed that the next
                                                          received byte will be correct even if there is no Framing Error.
                                                          Note: A framing error is associated with the character at the
                                                          to                                                                   */
      __I  uint32_t  BI         :  1;               /*!< Break Interrupt. When RXD is held in the spacing state (all
                                                         zeros) for one full character transmission (start, data, parity,
                                                          stop), a break interrupt occurs. Once the break condition has
                                                          been detected, the receiver goes idle until RXD goes to marking
                                                          state (all ones). A LSR read clears this status bit. The time
                                                          of break detection is dependent on FCR[0]. Note: The break interrupt
                                                          is associated with the character at the top of the USART RBR
                                                          FIFO.                                                                */
      __I  uint32_t  THRE       :  1;               /*!< Transmitter Holding Register Empty. THRE is set immediately
                                                         upon detection of an empty UART THR and is cleared on a UTHR
                                                          write.                                                               */
      __I  uint32_t  TEMT       :  1;               /*!< Transmitter Empty. TEMT is set when both UTHR and UTSR are empty;
                                                         TEMT is cleared when either the UTSR or the UTHR contain valid
                                                          data.                                                                */
      __I  uint32_t  RXFE       :  1;               /*!< Error in RX FIFO. ULSR[7] is set when a character with a RX
                                                         error such as framing error, parity error or break interrupt,
                                                          is loaded into the URBR. This bit is cleared when the ULSR register
                                                          is read and there are no subsequent errors in the UART FIFO.         */
    } ULSR_b;                                       /*!< BitSize                                                               */
  };
  
  union {
    __IO uint32_t  UACR;                            /*!< Auto-baud Control Register. Contains controls for the auto-baud
                                                         feature.                                                              */
    
    struct {
      __IO uint32_t  START      :  1;               /*!< This bit is automatically cleared after auto-baud completion.         */
      __IO uint32_t  MODE       :  1;               /*!< Auto-baud mode select bit.                                            */
           uint32_t             :  6;
      __IO uint32_t  ABEOINTCLR :  1;               /*!< End of auto-baud interrupt clear bit (write only accessible).         */
      __IO uint32_t  ABTOINTCLR :  1;               /*!< Auto-baud time-out interrupt clear bit (write only accessible).       */
    } UACR_b;                                       /*!< BitSize                                                               */
  };
  __I  uint32_t  RESERVED1[8];
  
  union {
    __IO uint32_t  URS485CTRL;                      /*!< RS-485/EIA-485 Control. Contains controls to configure various
                                                         aspects of RS-485/EIA-485 modes.                                      */
    
    struct {
      __IO uint32_t  NMMEN      :  1;               /*!< NMM enable.                                                           */
      __IO uint32_t  RXDIS      :  1;               /*!< Receiver enable.                                                      */
      __IO uint32_t  AADEN      :  1;               /*!< AAD enable.                                                           */
    } URS485CTRL_b;                                 /*!< BitSize                                                               */
  };
  
  union {
    __IO uint32_t  URS485ADRMATCH;                  /*!< RS-485/EIA-485 address match. Contains the address match value
                                                         for RS-485/EIA-485 mode.                                              */
    
    struct {
      __IO uint32_t  ADRMATCH   :  8;               /*!< Contains the address match value.                                     */
    } URS485ADRMATCH_b;                             /*!< BitSize                                                               */
  };
  __I  uint32_t  RESERVED2;
  
  union {
    __IO uint32_t  URCONDMA;                        /*!< UART Control DMA Register.                                            */
    
    struct {
           uint32_t             : 28;
      __IO uint32_t  DMARREQ    :  1;               /*!< DMA Receiver Request. (Read only)                                     */
      __IO uint32_t  DMATREQ    :  1;               /*!< DMA Transmitter Request. (Read only)                                  */
           uint32_t             :  1;
      __IO uint32_t  DMAE       :  1;               /*!< UART DMA Enable.                                                      */
    } URCONDMA_b;                                   /*!< BitSize                                                               */
  };
} OB_UART_Type;


/* ================================================================================ */
/* ================                      CT16B                     ================ */
/* ================================================================================ */


/**
  * @brief 16-bit counter/timer 0/1 (CT16B)
  */

typedef struct {                                    /*!< CT16B Structure                                                       */
  
  union {
    __IO uint32_t  TMR16IR;                         /*!< Interrupt Register (IR). The IR can be written to clear interrupts.
                                                         The IR can be read to identify which of five possible interrupt
                                                          sources are pending.                                                 */
    
    struct {
      __IO uint32_t  MR0INT     :  1;               /*!< Interrupt flag for match channel 0.                                   */
      __IO uint32_t  MR1INT     :  1;               /*!< Interrupt flag for match channel 1.                                   */
      __IO uint32_t  MR2INT     :  1;               /*!< Interrupt flag for match channel 2.                                   */
      __IO uint32_t  MR3INT     :  1;               /*!< Interrupt flag for match channel 3.                                   */
      __IO uint32_t  CR0INT     :  1;               /*!< Interrupt flag for capture the channel 0 event.                       */
      __IO uint32_t  CR1INT     :  1;               /*!< Interrupt flag for capture the channel 1 event.                       */
      __IO uint32_t  CR2INT     :  1;               /*!< Interrupt flag for capture the channel 2 event.                       */
    } TMR16IR_b;                                    /*!< BitSize                                                               */
  };
  
  union {
    __IO uint32_t  TMR16TCR;                        /*!< Timer Control Register (TCR). The TCR is used to control the
                                                         Timer Counter functions. The Timer Counter can be disabled or
                                                          reset through the TCR.                                               */
    
    struct {
      __IO uint32_t  CEN        :  1;               /*!< Counter Enable. When one, the Timer Counter and Prescale Counter
                                                         are enabled for counting.                                             */
      __IO uint32_t  CRST       :  1;               /*!< Counter Reset. When one, the Timer Counter and the Prescale
                                                         Counter are synchronously reset on the next positive edge of
                                                          PCLK. The counters remain reset until TCR[1] is returned to
                                                          zero.                                                                */
    } TMR16TCR_b;                                   /*!< BitSize                                                               */
  };
  
  union {
    __IO uint32_t  TMR16TC;                         /*!< Timer Counter (TC). The 16-bit TC is incremented every PR+1
                                                         cycles of PCLK. The TC is controlled through the TCR.                 */
    
    struct {
      __IO uint32_t  TC         : 16;               /*!< Timer counter value.                                                  */
    } TMR16TC_b;                                    /*!< BitSize                                                               */
  };
  
  union {
    __IO uint32_t  TMR16PR;                         /*!< Prescale Register (PR). When the Prescale Counter (below) is
                                                         equal to this value, the next clock increments the TC and clears
                                                          the PC.                                                              */
    
    struct {
      __IO uint32_t  PR         : 16;               /*!< Prescale max value.                                                   */
    } TMR16PR_b;                                    /*!< BitSize                                                               */
  };
  
  union {
    __IO uint32_t  TMR16PC;                         /*!< Prescale Counter (PC). The 16-bit PC is a counter which is incremented
                                                         to the value stored in PR. When the value in PR is reached,
                                                          the TC is incremented and the PC is cleared. The PC is observable
                                                          and controllable through the bus interface.                          */
    
    struct {
      __IO uint32_t  PC         : 16;               /*!< Prescale counter value.                                               */
    } TMR16PC_b;                                    /*!< BitSize                                                               */
  };
  
  union {
    __IO uint32_t  TMR16MCR;                        /*!< Match Control Register (MCR). The MCR is used to control if
                                                         an interrupt is generated and if the TC is reset when a Match
                                                          occurs.                                                              */
    
    struct {
      __IO uint32_t  MR0I       :  1;               /*!< Interrupt on MR0: an interrupt is generated when MR0 matches
                                                         the value in the TC.                                                  */
      __IO uint32_t  MR0R       :  1;               /*!< Reset on MR0: the TC will be reset if MR0 matches it.                 */
      __IO uint32_t  MR0S       :  1;               /*!< Stop on MR0: the TC and PC will be stopped and TCR[0] will be
                                                         set to 0 if MR0 matches the TC.                                       */
      __IO uint32_t  MR1I       :  1;               /*!< Interrupt on MR1: an interrupt is generated when MR1 matches
                                                         the value in the TC.                                                  */
      __IO uint32_t  MR1R       :  1;               /*!< Reset on MR1: the TC will be reset if MR1 matches it.                 */
      __IO uint32_t  MR1S       :  1;               /*!< Stop on MR1: the TC and PC will be stopped and TCR[0] will be
                                                         set to 0 if MR1 matches the TC.                                       */
      __IO uint32_t  MR2I       :  1;               /*!< Interrupt on MR2: an interrupt is generated when MR2 matches
                                                         the value in the TC.                                                  */
      __IO uint32_t  MR2R       :  1;               /*!< Reset on MR2: the TC will be reset if MR2 matches it.                 */
      __IO uint32_t  MR2S       :  1;               /*!< Stop on MR2: the TC and PC will be stopped and TCR[0] will be
                                                         set to 0 if MR2 matches the TC.                                       */
      __IO uint32_t  MR3I       :  1;               /*!< Interrupt on MR3: an interrupt is generated when MR3 matches
                                                         the value in the TC.                                                  */
      __IO uint32_t  MR3R       :  1;               /*!< Reset on MR3: the TC will be reset if MR3 matches it.                 */
      __IO uint32_t  MR3S       :  1;               /*!< Stop on MR3: the TC and PC will be stopped and TCR[0] will be
                                                         set to 0 if MR3 matches the TC.                                       */
    } TMR16MCR_b;                                   /*!< BitSize                                                               */
  };
  
  union {
    __IO uint32_t  TMR16MR0;                        /*!< Match Register x(MRx). MRx can be enabled through the MCR to
                                                         reset the TC, stop both the TC and PC, and/or generate an interrupt
                                                          every time MRx matches the TC.                                       */
    
    struct {
      __IO uint32_t  MATCH      : 16;               /*!< Timer counter match value.                                            */
    } TMR16MR0_b;                                   /*!< BitSize                                                               */
  };
  
  union {
    __IO uint32_t  TMR16MR1;                        /*!< Match Register x(MRx). MRx can be enabled through the MCR to
                                                         reset the TC, stop both the TC and PC, and/or generate an interrupt
                                                          every time MRx matches the TC.                                       */
    
    struct {
      __IO uint32_t  MATCH      : 16;               /*!< Timer counter match value.                                            */
    } TMR16MR1_b;                                   /*!< BitSize                                                               */
  };
  
  union {
    __IO uint32_t  TMR16MR2;                        /*!< Match Register x(MRx). MRx can be enabled through the MCR to
                                                         reset the TC, stop both the TC and PC, and/or generate an interrupt
                                                          every time MRx matches the TC.                                       */
    
    struct {
      __IO uint32_t  MATCH      : 16;               /*!< Timer counter match value.                                            */
    } TMR16MR2_b;                                   /*!< BitSize                                                               */
  };
  
  union {
    __IO uint32_t  TMR16MR3;                        /*!< Match Register x(MRx). MRx can be enabled through the MCR to
                                                         reset the TC, stop both the TC and PC, and/or generate an interrupt
                                                          every time MRx matches the TC.                                       */
    
    struct {
      __IO uint32_t  MATCH      : 16;               /*!< Timer counter match value.                                            */
    } TMR16MR3_b;                                   /*!< BitSize                                                               */
  };
  
  union {
    __IO uint32_t  TMR16CCR;                        /*!< Capture Control Register (CCR). The CCR controls which edges
                                                         of the capture inputs are used to load the Capture Registers
                                                          and whether or not an interrupt is generated when a capture
                                                          takes place.                                                         */
    
    struct {
      __IO uint32_t  CAP0RE     :  1;               /*!< Capture on CT16Bn_CAP0 rising edge: a sequence of 0 then 1 on
                                                         CT16Bn_CAP0 will cause CR0 to be loaded with the contents of
                                                          TC.                                                                  */
      __IO uint32_t  CAP0FE     :  1;               /*!< Capture on CT16Bn_CAP0 falling edge: a sequence of 1 then 0
                                                         on CT16Bn_CAP0 will cause CR0 to be loaded with the contents
                                                          of TC.                                                               */
      __IO uint32_t  CAP0I      :  1;               /*!< Interrupt on CT16Bn_CAP0 event: a CR0 load due to a CT16Bn_CAP0
                                                         event will generate an interrupt.                                     */
      __IO uint32_t  CAP1RE     :  1;               /*!< Capture on CT16Bn_CAP1 rising edge: a sequence of 0 then 1 on
                                                         CT16Bn_CAP1 will cause CR1 to be loaded with the contents of
                                                          TC.                                                                  */
      __IO uint32_t  CAP1FE     :  1;               /*!< Capture on CT16Bn_CAP1 falling edge: a sequence of 1 then 0
                                                         on CT16Bn_CAP1 will cause CR1 to be loaded with the contents
                                                          of TC.                                                               */
      __IO uint32_t  CAP1I      :  1;               /*!< Interrupt on CT16Bn_CAP1 event: a CR1 load due to a CT16Bn_CAP1
                                                         event will generate an interrupt.                                     */
      __IO uint32_t  CAP2RE     :  1;               /*!< Capture on CT16Bn_CAP2 rising edge: a sequence of 0 then 1 on
                                                         CT16Bn_CAP2 will cause CR2 to be loaded with the contents of
                                                          TC.                                                                  */
      __IO uint32_t  CAP2FE     :  1;               /*!< Capture on CT16Bn_CAP2 falling edge: a sequence of 1 then 0
                                                         on CT16Bn_CAP2 will cause CR2 to be loaded with the contents
                                                          of TC.                                                               */
      __IO uint32_t  CAP2I      :  1;               /*!< Interrupt on CT16Bn_CAP2 event: a CR1 load due to a CT16Bn_CAP2
                                                         event will generate an interrupt.                                     */
           uint32_t             : 19;
      __IO uint32_t  CAP0SS     :  1;               /*!< Select the CT16Bn_CAP0 source                                         */
      __IO uint32_t  CAP1SS     :  1;               /*!< Select the CT16Bn _CAP1 source                                        */
      __IO uint32_t  CAP2SS     :  1;               /*!< Select the CT16Bn _CAP2 source                                        */
    } TMR16CCR_b;                                   /*!< BitSize                                                               */
  };
  
  union {
    __I  uint32_t  TMR16CR0;                        /*!< Capture Register x(CRx). CRx is loaded with the value of TC
                                                         when there is an event on the CT16Bn_CAPx input.                      */
    
    struct {
      __I  uint32_t  CAP        : 16;               /*!< Timer counter capture value.                                          */
    } TMR16CR0_b;                                   /*!< BitSize                                                               */
  };
  
  union {
    __I  uint32_t  TMR16CR1;                        /*!< Capture Register x(CRx). CRx is loaded with the value of TC
                                                         when there is an event on the CT16Bn_CAPx input.                      */
    
    struct {
      __I  uint32_t  CAP        : 16;               /*!< Timer counter capture value.                                          */
    } TMR16CR1_b;                                   /*!< BitSize                                                               */
  };
  
  union {
    __I  uint32_t  TMR16CR2;                        /*!< Capture Register x(CRx). CRx is loaded with the value of TC
                                                         when there is an event on the CT16Bn_CAPx input.                      */
    
    struct {
      __I  uint32_t  CAP        : 16;               /*!< Timer counter capture value.                                          */
    } TMR16CR2_b;                                   /*!< BitSize                                                               */
  };
  __I  uint32_t  RESERVED0;
  
  union {
    __IO uint32_t  TMR16EMR;                        /*!< External Match Register (EMR). The EMR controls the match function
                                                         and the external match pins CT16Bn_MAT[2:0].                          */
    
    struct {
      __IO uint32_t  EM0        :  1;               /*!< External Match 0. This bit reflects the state of output CT16B0_MAT0/CT16B1
                                                         _MAT0, whether or not this output is connected to its pin. When
                                                         a match occurs between the TC and MR0, this bit can either toggle,
                                                          go LOW, go HIGH, or do nothing. Bits EMR[5:4] control the functionality
                                                          of this output. This bit is driven to the CT16B0_MAT0/CT16B1_MAT0
                                                          pins if the match function is selected in the IOCON registers
                                                          (0 = LOW, 1 = HIGH).                                                 */
      __IO uint32_t  EM1        :  1;               /*!< External Match 1. This bit reflects the state of output CT16B0_MAT1/CT16B1
                                                         _MAT1, whether or not this output is connected to its pin. When
                                                         a match occurs between the TC and MR1, this bit can either toggle,
                                                          go LOW, go HIGH, or do nothing. Bits EMR[7:6] control the functionality
                                                          of this output. This bit is driven to the CT16B0_MAT1/CT16B1_MAT1
                                                          pins if the match function is selected in the IOCON registers
                                                          (0 = LOW, 1 = HIGH).                                                 */
      __IO uint32_t  EM2        :  1;               /*!< External Match 2. This bit reflects the state of output match
                                                         channel 2, whether or not this output is connected to its pin.
                                                          When a match occurs between the TC and MR2, this bit can either
                                                          toggle, go LOW, go HIGH, or do nothing. Bits EMR[9:8] control
                                                          the functionality of this output. Note that on counter/timer
                                                          0 this match channel is not pinned out. This bit is driven to
                                                          the CT16B1_MAT2 pin if the match function is selected in the
                                                          IOCON registers (0 = LOW, 1 = HIGH).                                 */
      __IO uint32_t  EM3        :  1;               /*!< External Match 3. This bit reflects the state of output of match
                                                         channel 3. When a match occurs between the TC and MR3, this
                                                          bit can either toggle, go LOW, go HIGH, or do nothing. Bits
                                                          EMR[11:10] control the functionality of this output. There is
                                                          no output pin available for this channel on either of the 16-bit
                                                          timers.                                                              */
      __IO uint32_t  EMC0       :  2;               /*!< External Match Control 0. Determines the functionality of External
                                                         Match 0.                                                              */
      __IO uint32_t  EMC1       :  2;               /*!< External Match Control 1. Determines the functionality of External
                                                         Match 1.                                                              */
      __IO uint32_t  EMC2       :  2;               /*!< External Match Control 2. Determines the functionality of External
                                                         Match 2.                                                              */
      __IO uint32_t  EMC3       :  2;               /*!< External Match Control 3. Determines the functionality of External
                                                         Match 3.                                                              */
    } TMR16EMR_b;                                   /*!< BitSize                                                               */
  };
  __I  uint32_t  RESERVED1[12];
  
  union {
    __IO uint32_t  TMR16CTCR;                       /*!< Count Control Register (CTCR). The CTCR selects between Timer
                                                         and Counter mode, and in Counter mode selects the signal and
                                                          edge(s) for counting.                                                */
    
    struct {
      __IO uint32_t  CTM        :  2;               /*!< Counter/Timer Mode. This field selects which rising PCLK edges
                                                         can increment Timer Prescale Counter (PC), or clear PC and increment
                                                          Timer Counter (TC).                                                  */
      __IO uint32_t  CIS        :  2;               /*!< Count Input Select. In counter mode (when bits 1:0 in this register
                                                         are not 00), these bits select which CAP pin is sampled for
                                                          clocking. Note: If Counter mode is selected in the CTCR register,
                                                          bits 2:0 in the Capture Control Register (CCR) must be programmed
                                                          as 000.                                                              */
      __IO uint32_t  ENCC       :  1;               /*!< Setting this bit to 1 enables clearing of the timer and the
                                                         prescaler when the capture-edge event specified in bits 7:5
                                                          occurs.                                                              */
      __IO uint32_t  SELCC      :  3;               /*!< When bit 4 is one, these bits select which capture input edge
                                                         will cause the timer and prescaler to be cleared. These bits
                                                          have no effect when bit 4 is zero.                                   */
    } TMR16CTCR_b;                                  /*!< BitSize                                                               */
  };
  
  union {
    __IO uint32_t  TMR16PWMC;                       /*!< PWM Control Register (PWMCON). The PWMCON enables PWM mode for
                                                         the external match pins CT16Bn_MAT[2:0].                              */
    
    struct {
      __IO uint32_t  PWMEN0     :  1;               /*!< PWM mode enable for channel0.                                         */
      __IO uint32_t  PWMEN1     :  1;               /*!< PWM mode enable for channel1.                                         */
      __IO uint32_t  PWMEN2     :  1;               /*!< PWM mode enable for channel2.                                         */
      __IO uint32_t  PWMEN3     :  1;               /*!< PWM mode enable for channel3. Note: It is recommended to use
                                                         match channel 3 to set the PWM cycle because it is not pinned
                                                          out.                                                                 */
    } TMR16PWMC_b;                                  /*!< BitSize                                                               */
  };
} OB_CT16B_Type;


/* ================================================================================ */
/* ================                      PWMTM                     ================ */
/* ================================================================================ */


/**
  * @brief PWMTM (PWMTM)
  */

typedef struct {                                    /*!< PWMTM Structure                                                       */
  
  union {
    __IO uint32_t  CON;                             /*!< The PWM Timer                                                         */
    
    struct {
      __IO uint32_t  EN0        :  1;               /*!< PWM Timer 0 Function enable                                           */
      __IO uint32_t  PWMTMIO    :  1;               /*!< PWM Timer 0 output initial value                                      */
      __IO uint32_t  PWMTMO0    :  1;               /*!< PWM Timer 0 output value                                              */
      __IO uint32_t  IEEN0      :  1;               /*!< PWM Timer 0 Interrupt enable                                          */
           uint32_t             :  4;
      __IO uint32_t  EN1        :  1;               /*!< PWM Timer 1 Function enable                                           */
      __IO uint32_t  PWMTMI1    :  1;               /*!< PWM Timer 1 output initial value                                      */
      __IO uint32_t  PWMTMO1    :  1;               /*!< PWM Timer 1 output value                                              */
      __IO uint32_t  IEEN1      :  1;               /*!< PWM Timer 1 Interrupt enable                                          */
    } CON_b;                                        /*!< BitSize                                                               */
  };
  
  union {
    __IO uint32_t  IEST;                            /*!< PWM Timer 0/1 Interrupt status                                        */
    
    struct {
      __IO uint32_t  IEST0      :  1;               /*!< PWM Timer 0 Interrupt status                                          */
           uint32_t             :  7;
      __IO uint32_t  IEST1      :  1;               /*!< PWM Timer 1 Interrupt status                                          */
    } IEST_b;                                       /*!< BitSize                                                               */
  };
  
  union {
    __IO uint32_t  IESTC;                           /*!< PWM Timer 1/0 status Clear Register                                   */
    
    struct {
      __IO uint32_t  IEST0      :  1;               /*!< PWM Timer 0 Interrupt status                                          */
           uint32_t             :  7;
      __IO uint32_t  IEST1      :  1;               /*!< PWM Timer 1 Interrupt status                                          */
    } IESTC_b;                                      /*!< BitSize                                                               */
  };
  __I  uint32_t  RESERVED0;
  
  union {
    __IO uint32_t  MAX0;                            /*!< PWM Timer 0 Maximum value                                             */
    
    struct {
      __IO uint32_t  PWMTMMAX0  : 16;               /*!< PWM Timer 0 Maximum value                                             */
    } MAX0_b;                                       /*!< BitSize                                                               */
  };
  
  union {
    __IO uint32_t  CNT0;                            /*!< PWM Timer 0 counter value                                             */
    
    struct {
      __IO uint32_t  PWMTMCNT   : 16;               /*!< PWM Timer 0 counter value                                             */
    } CNT0_b;                                       /*!< BitSize                                                               */
  };
  
  union {
    __IO uint32_t  CMP0;                            /*!< PWM Timer 0 compare value                                             */
    
    struct {
      __IO uint32_t  PWMTMCMP   : 16;               /*!< PWM Timer 0 compare value                                             */
    } CMP0_b;                                       /*!< BitSize                                                               */
  };
  __I  uint32_t  RESERVED1;
  
  union {
    __IO uint32_t  MAX1;                            /*!< PWM Timer 0 Maximum value                                             */
    
    struct {
      __IO uint32_t  PWMTMMAX1  : 16;               /*!< PWM Timer 1 Maximum value                                             */
    } MAX1_b;                                       /*!< BitSize                                                               */
  };
  
  union {
    __IO uint32_t  CNT1;                            /*!< PWM Timer 1 counter value                                             */
    
    struct {
      __IO uint32_t  PWMTMCNT   : 16;               /*!< PWM Timer 1 counter value                                             */
    } CNT1_b;                                       /*!< BitSize                                                               */
  };
  
  union {
    __IO uint32_t  CMP1;                            /*!< PWM Timer 1 compare value                                             */
    
    struct {
      __IO uint32_t  PWMTMCPM   : 16;               /*!< PWM Timer 1 compare value                                             */
    } CMP1_b;                                       /*!< BitSize                                                               */
  };
} OB_PWMTM_Type;


/* ================================================================================ */
/* ================                      CT32B                     ================ */
/* ================================================================================ */


/**
  * @brief 32-bit counter/timer 0/1 (CT32B)
  */

typedef struct {                                    /*!< CT32B Structure                                                       */
  
  union {
    __IO uint32_t  TMR32IR;                         /*!< Interrupt Register (IR). The IR can be written to clear interrupts.
                                                         The IR can be read to identify which of five possible interrupt
                                                          sources are pending.                                                 */
    
    struct {
      __IO uint32_t  MR0INT     :  1;               /*!< Interrupt flag for match channel 0.                                   */
      __IO uint32_t  MR1INT     :  1;               /*!< Interrupt flag for match channel 1.                                   */
      __IO uint32_t  MR2INT     :  1;               /*!< Interrupt flag for match channel 2.                                   */
      __IO uint32_t  MR3INT     :  1;               /*!< Interrupt flag for match channel 3.                                   */
      __IO uint32_t  CR0INT     :  1;               /*!< Interrupt flag for capture the channel 0 event.                       */
      __IO uint32_t  CR1INT     :  1;               /*!< Interrupt flag for capture the channel 1 event.                       */
      __IO uint32_t  CR2INT     :  1;               /*!< Interrupt flag for capture the channel 2 event.                       */
    } TMR32IR_b;                                    /*!< BitSize                                                               */
  };
  
  union {
    __IO uint32_t  TMR32TCR;                        /*!< Timer Control Register (TCR). The TCR is used to control the
                                                         Timer Counter functions. The Timer Counter can be disabled or
                                                          reset through the TCR.                                               */
    
    struct {
      __IO uint32_t  CEN        :  1;               /*!< When one, the Timer Counter and Prescale Counter are enabled
                                                         for counting. When zero, the counters are disabled.                   */
      __IO uint32_t  CRST       :  1;               /*!< When one, the Timer Counter and the Prescale Counter are synchronously
                                                         reset on the next positive edge of PCLK. The counters remain
                                                          reset until TCR[1] is returned to zero.                              */
    } TMR32TCR_b;                                   /*!< BitSize                                                               */
  };
  
  union {
    __IO uint32_t  TMR32TC;                         /*!< Timer Counter (TC). The 32-bit TC is incremented every PR+1
                                                         cycles of PCLK. The TC is controlled through the TCR.                 */
    
    struct {
      __IO uint32_t  TC         : 32;               /*!< Timer counter value.                                                  */
    } TMR32TC_b;                                    /*!< BitSize                                                               */
  };
  
  union {
    __IO uint32_t  TMR32PR;                         /*!< Prescale Register (PR). When the Prescale Counter (below) is
                                                         equal to this value, the next clock increments the TC and clears
                                                          the PC.                                                              */
    
    struct {
      __IO uint32_t  PR         : 32;               /*!< Prescale max value.                                                   */
    } TMR32PR_b;                                    /*!< BitSize                                                               */
  };
  
  union {
    __IO uint32_t  TMR32PC;                         /*!< Prescale Counter (PC). The 32-bit PC is a counter which is incremented
                                                         to the value stored in PR. When the value in PR is reached,
                                                          the TC is incremented and the PC is cleared. The PC is observable
                                                          and controllable through the bus interface.                          */
    
    struct {
      __IO uint32_t  PC         : 32;               /*!< Prescale counter value.                                               */
    } TMR32PC_b;                                    /*!< BitSize                                                               */
  };
  
  union {
    __IO uint32_t  TMR32MCR;                        /*!< Match Control Register (MCR). The MCR is used to control if
                                                         an interrupt is generated and if the TC is reset when a Match
                                                          occurs.                                                              */
    
    struct {
      __IO uint32_t  MR0I       :  1;               /*!< Interrupt on MR0: an interrupt is generated when MR0 matches
                                                         the value in the TC.                                                  */
      __IO uint32_t  MR0R       :  1;               /*!< Reset on MR0: the TC will be reset if MR0 matches it.                 */
      __IO uint32_t  MR0S       :  1;               /*!< Stop on MR0: the TC and PC will be stopped and TCR[0] will be
                                                         set to 0 if MR0 matches the TC.                                       */
      __IO uint32_t  MR1I       :  1;               /*!< Interrupt on MR1: an interrupt is generated when MR1 matches
                                                         the value in the TC.                                                  */
      __IO uint32_t  MR1R       :  1;               /*!< Reset on MR1: the TC will be reset if MR1 matches it.                 */
      __IO uint32_t  MR1S       :  1;               /*!< Stop on MR1: the TC and PC will be stopped and TCR[0] will be
                                                         set to 0 if MR1 matches the TC.                                       */
      __IO uint32_t  MR2I       :  1;               /*!< Interrupt on MR2: an interrupt is generated when MR2 matches
                                                         the value in the TC.                                                  */
      __IO uint32_t  MR2R       :  1;               /*!< Reset on MR2: the TC will be reset if MR2 matches it.                 */
      __IO uint32_t  MR2S       :  1;               /*!< Stop on MR2: the TC and PC will be stopped and TCR[0] will be
                                                         set to 0 if MR2 matches the TC.                                       */
      __IO uint32_t  MR3I       :  1;               /*!< Interrupt on MR3: an interrupt is generated when MR3 matches
                                                         the value in the TC.                                                  */
      __IO uint32_t  MR3R       :  1;               /*!< Reset on MR3: the TC will be reset if MR3 matches it.                 */
      __IO uint32_t  MR3S       :  1;               /*!< Stop on MR3: the TC and PC will be stopped and TCR[0] will be
                                                         set to 0 if MR3 matches the TC.                                       */
    } TMR32MCR_b;                                   /*!< BitSize                                                               */
  };
  
  union {
    __IO uint32_t  TMR32MR0;                        /*!< Match Register x(MRx). MRx can be enabled through the MCR to
                                                         reset the TC, stop both the TC and PC, and/or generate an interrupt
                                                          every time MRx matches the TC.                                       */
    
    struct {
      __IO uint32_t  MATCH      : 32;               /*!< Timer counter match value.                                            */
    } TMR32MR0_b;                                   /*!< BitSize                                                               */
  };
  
  union {
    __IO uint32_t  TMR32MR1;                        /*!< Match Register x(MRx). MRx can be enabled through the MCR to
                                                         reset the TC, stop both the TC and PC, and/or generate an interrupt
                                                          every time MRx matches the TC.                                       */
    
    struct {
      __IO uint32_t  MATCH      : 32;               /*!< Timer counter match value.                                            */
    } TMR32MR1_b;                                   /*!< BitSize                                                               */
  };
  
  union {
    __IO uint32_t  TMR32MR2;                        /*!< Match Register x(MRx). MRx can be enabled through the MCR to
                                                         reset the TC, stop both the TC and PC, and/or generate an interrupt
                                                          every time MRx matches the TC.                                       */
    
    struct {
      __IO uint32_t  MATCH      : 32;               /*!< Timer counter match value.                                            */
    } TMR32MR2_b;                                   /*!< BitSize                                                               */
  };
  
  union {
    __IO uint32_t  TMR32MR3;                        /*!< Match Register x(MRx). MRx can be enabled through the MCR to
                                                         reset the TC, stop both the TC and PC, and/or generate an interrupt
                                                          every time MRx matches the TC.                                       */
    
    struct {
      __IO uint32_t  MATCH      : 32;               /*!< Timer counter match value.                                            */
    } TMR32MR3_b;                                   /*!< BitSize                                                               */
  };
  
  union {
    __IO uint32_t  TMR32CCR;                        /*!< Capture Control Register (CCR). The CCR controls which edges
                                                         of the capture inputs are used to load the Capture Registers
                                                          and whether or not an interrupt is generated when a capture
                                                          takes place.                                                         */
    
    struct {
      __IO uint32_t  CAP0RE     :  1;               /*!< Capture on CT32Bn_CAP0 rising edge: a sequence of 0 then 1 on
                                                         CT32Bn_CAP0 will cause CR0 to be loaded with the contents of
                                                          TC.                                                                  */
      __IO uint32_t  CAP0FE     :  1;               /*!< Capture on CT32Bn_CAP0 falling edge: a sequence of 1 then 0
                                                         on CT32Bn_CAP0 will cause CR0 to be loaded with the contents
                                                          of TC.                                                               */
      __IO uint32_t  CAP0I      :  1;               /*!< Interrupt on CT32Bn_CAP0 event: a CR0 load due to a CT32Bn_CAP0
                                                         event will generate an interrupt.                                     */
      __IO uint32_t  CAP1RE     :  1;               /*!< Capture on CT32Bn_CAP1 rising edge: a sequence of 0 then 1 on
                                                         CT32Bn_CAP1 will cause CR1 to be loaded with the contents of
                                                          TC.                                                                  */
      __IO uint32_t  CAP1FE     :  1;               /*!< Capture on CT32Bn_CAP1 falling edge: a sequence of 1 then 0
                                                         on CT32Bn_CAP1 will cause CR1 to be loaded with the contents
                                                          of TC.                                                               */
      __IO uint32_t  CAP1I      :  1;               /*!< Interrupt on CT32Bn_CAP1 event: a CR1 load due to a CT32Bn_CAP1
                                                         event will generate an interrupt.                                     */
      __IO uint32_t  CAP2RE     :  1;               /*!< Capture on CT32Bn_CAP2 rising edge: a sequence of 0 then 1 on
                                                         CT32Bn_CAP2 will cause CR2 to be loaded with the contents of
                                                          TC.                                                                  */
      __IO uint32_t  CAP2FE     :  1;               /*!< Capture on CT32Bn_CAP2 falling edge: a sequence of 1 then 0
                                                         on CT32Bn_CAP2 will cause CR2 to be loaded with the contents
                                                          of TC.                                                               */
      __IO uint32_t  CAP2I      :  1;               /*!< Interrupt on CT32Bn_CAP2 event: a CR2 load due to a CT32Bn_CAP2
                                                         event will generate an interrupt.                                     */
           uint32_t             : 19;
      __IO uint32_t  CAP0SS     :  1;               /*!< Select the CT326Bn_CAP0 source                                        */
      __IO uint32_t  CAP1SS     :  1;               /*!< Select the CT326Bn _CAP2 source                                       */
      __IO uint32_t  CAP2SS     :  1;               /*!< Select the CT326Bn _CAP2 source                                       */
    } TMR32CCR_b;                                   /*!< BitSize                                                               */
  };
  
  union {
    __I  uint32_t  TMR32CR0;                        /*!< Capture Register x(CRx). CRx is loaded with the value of TC
                                                         when there is an event on the CT32Bn_CAPx input.                      */
    
    struct {
      __I  uint32_t  CAP        : 32;               /*!< Timer counter capture value.                                          */
    } TMR32CR0_b;                                   /*!< BitSize                                                               */
  };
  
  union {
    __I  uint32_t  TMR32CR1;                        /*!< Capture Register x(CRx). CRx is loaded with the value of TC
                                                         when there is an event on the CT32Bn_CAPx input.                      */
    
    struct {
      __I  uint32_t  CAP        : 32;               /*!< Timer counter capture value.                                          */
    } TMR32CR1_b;                                   /*!< BitSize                                                               */
  };
  
  union {
    __I  uint32_t  TMR32CR2;                        /*!< Capture Register x(CRx). CRx is loaded with the value of TC
                                                         when there is an event on the CT32Bn_CAPx input.                      */
    
    struct {
      __I  uint32_t  CAP        : 32;               /*!< Timer counter capture value.                                          */
    } TMR32CR2_b;                                   /*!< BitSize                                                               */
  };
  __I  uint32_t  RESERVED0;
  
  union {
    __IO uint32_t  TMR32EMR;                        /*!< External Match Register (EMR). The EMR controls the match function
                                                         and the external match pins CT32Bn_MAT[2:0].                          */
    
    struct {
      __IO uint32_t  EM0        :  1;               /*!< External Match 0. This bit reflects the state of output CT32B0_MAT0/CT32B1
                                                         _MAT0, whether or not this output is connected to its pin. When
                                                         a match occurs between the TC and MR0, this bit can either toggle,
                                                          go LOW, go HIGH, or do nothing. Bits EMR[5:4] control the functionality
                                                          of this output. This bit is driven to the CT32B0_MAT0/CT32B1_MAT0
                                                          pins if the match function is selected in the IOCON registers
                                                          (0 = LOW, 1 = HIGH).                                                 */
      __IO uint32_t  EM1        :  1;               /*!< External Match 1. This bit reflects the state of output CT32B0_MAT1/CT32B1
                                                         _MAT1, whether or not this output is connected to its pin. When
                                                         a match occurs between the TC and MR1, this bit can either toggle,
                                                          go LOW, go HIGH, or do nothing. Bits EMR[7:6] control the functionality
                                                          of this output. This bit is driven to the CT32B0_MAT1/CT32B1_MAT1
                                                          pins if the match function is selected in the IOCON registers
                                                          (0 = LOW, 1 = HIGH).                                                 */
      __IO uint32_t  EM2        :  1;               /*!< External Match 2. This bit reflects the state of output CT32B0_MAT2/CT32B1
                                                         _MAT2, whether or not this output is connected to its pin. When
                                                         a match occurs between the TC and MR2, this bit can either toggle,
                                                          go LOW, go HIGH, or do nothing. Bits EMR[9:8] control the functionality
                                                          of this output. This bit is driven to the CT32B0_MAT2/CT32B1_MAT2
                                                          pins if the match function is selected in the IOCON registers
                                                          (0 = LOW, 1 = HIGH).                                                 */
      __IO uint32_t  EM3        :  1;               /*!< External Match 3. This bit reflects the state of output CT32Bn_MAT3,
                                                         whether or not this output is connected to its pin. When a match
                                                          occurs between the TC and MR3, this bit can either toggle, go
                                                          LOW, go HIGH, or do nothing. Bits EMR[11:10] control the functionality
                                                          of this output. This bit is driven to the CT32B0_MAT3/CT32B1_MAT3
                                                          pins if the match function is selected in the IOCON registers
                                                          (0 = LOW, 1 = HIGH).                                                 */
      __IO uint32_t  EMC0       :  2;               /*!< External Match Control 0. Determines the functionality of External
                                                         Match 0.                                                              */
      __IO uint32_t  EMC1       :  2;               /*!< External Match Control 1. Determines the functionality of External
                                                         Match 1.                                                              */
      __IO uint32_t  EMC2       :  2;               /*!< External Match Control 2. Determines the functionality of External
                                                         Match 2.                                                              */
      __IO uint32_t  EMC3       :  2;               /*!< External Match Control 3. Determines the functionality of External
                                                         Match 3.                                                              */
    } TMR32EMR_b;                                   /*!< BitSize                                                               */
  };
  __I  uint32_t  RESERVED1[12];
  
  union {
    __IO uint32_t  TMR32CTCR;                       /*!< Count Control Register (CTCR). The CTCR selects between Timer
                                                         and Counter mode, and in Counter mode selects the signal and
                                                          edge(s) for counting.                                                */
    
    struct {
      __IO uint32_t  CTM        :  2;               /*!< Counter/Timer Mode. This field selects which rising PCLK edges
                                                         can increment Timer Prescale Counter (PC), or clear PC and increment
                                                          Timer Counter (TC).                                                  */
      __IO uint32_t  CIS        :  2;               /*!< Count Input Select. In counter mode (when bits 1:0 in this register
                                                         are not 00), these bits select which CAP pin is sampled for
                                                          clocking. Note: If Counter mode is selected in the CTCR register,
                                                          bits 2:0 in the Capture Control Register (CCR) must be programmed
                                                          as 000. Note: If Counter mode is selected in the TnCTCR, the
                                                          3 bits for that input in the Capture Control Register (TnCCR)
                                                          must be programmed as 000.                                           */
      __IO uint32_t  ENCC       :  1;               /*!< Setting this bit to 1 enables clearing of the timer and the
                                                         prescaler when the capture-edge event specified in bits 7:5
                                                          occurs.                                                              */
      __IO uint32_t  SELCC      :  3;               /*!< When bit 4 is one, these bits select which capture input edge
                                                         will cause the timer and prescaler to be cleared. These bits
                                                          have no effect when bit 4 is zero.                                   */
    } TMR32CTCR_b;                                  /*!< BitSize                                                               */
  };
  
  union {
    __IO uint32_t  TMR32PWMC;                       /*!< PWM Control Register (PWMCON). The PWMCON enables PWM mode for
                                                         the external match pins CT32Bn_MAT[2:0].                              */
    
    struct {
      __IO uint32_t  PWMEN0     :  1;               /*!< PWM mode enable for channel0.                                         */
      __IO uint32_t  PWMEN1     :  1;               /*!< PWM mode enable for channel1.                                         */
      __IO uint32_t  PWMEN2     :  1;               /*!< PWM mode enable for channel2.                                         */
      __IO uint32_t  PWMEN3     :  1;               /*!< PWM mode enable for channel3. Note: It is recommended to use
                                                         match channel 3 to set the PWM cycle because it is not pinned
                                                          out.                                                                 */
    } TMR32PWMC_b;                                  /*!< BitSize                                                               */
  };
} OB_CT32B_Type;


/* ================================================================================ */
/* ================                       ADC                      ================ */
/* ================================================================================ */


/**
  * @brief Analog to Digital Convert (ADC)
  */

typedef struct {                                    /*!< ADC Structure                                                         */
  
  union {
    __IO uint32_t  CON;                             /*!< ADC Control Register                                                  */
    
    struct {
      __IO uint32_t  START0     :  1;               /*!< When this bit is set, the ADC 0 will be start conversion. (Auto
                                                         clears by hardware)                                                   */
      __IO uint32_t  EN0        :  1;               /*!< ADC 0 controller bit                                                  */
           uint32_t             :  2;
      __IO uint32_t  MODE0      :  1;               /*!< ADC 0 mode selection                                                  */
      __IO uint32_t  RSTRSIN0   :  1;               /*!< Reset the counter of RSIN0                                            */
           uint32_t             :  2;
      __IO uint32_t  START1     :  1;               /*!< When this bit is set, the ADC1 will be start conversion. (Auto
                                                         clears by hardware)                                                   */
      __IO uint32_t  EN1        :  1;               /*!< ADC 1 controller bit                                                  */
           uint32_t             :  2;
      __IO uint32_t  MODE1      :  1;               /*!< ADC 1 mode selection                                                  */
      __IO uint32_t  RSTRSIN1   :  1;               /*!< Reset the counter of RSIN1                                            */
    } CON_b;                                        /*!< BitSize                                                               */
  };
  
  union {
    __IO uint32_t  A0SC0;                           /*!< ADC 0 Sample Sequence Register                                        */
    
    struct {
      __IO uint32_t  SC1        :  5;               /*!< The 1st Sample Input Select                                           */
           uint32_t             :  3;
      __IO uint32_t  SC2        :  5;               /*!< The 2st Sample Input Select                                           */
           uint32_t             :  3;
      __IO uint32_t  SC3        :  5;               /*!< The 3st Sample Input Select                                           */
           uint32_t             :  3;
      __IO uint32_t  SC4        :  5;               /*!< The 4st Sample Input Select                                           */
    } A0SC0_b;                                      /*!< BitSize                                                               */
  };
  
  union {
    __IO uint32_t  A0SC1;                           /*!< ADC 0 Sample Sequence Register                                        */
    
    struct {
      __IO uint32_t  SC5        :  5;               /*!< The 5st Sample Input Select                                           */
           uint32_t             :  3;
      __IO uint32_t  SC6        :  5;               /*!< The 6st Sample Input Select                                           */
           uint32_t             :  3;
      __IO uint32_t  SC7        :  5;               /*!< The 7st Sample Input Select                                           */
           uint32_t             :  3;
      __IO uint32_t  SC8        :  5;               /*!< The 8st Sample Input Select                                           */
    } A0SC1_b;                                      /*!< BitSize                                                               */
  };
  
  union {
    __IO uint32_t  SL0;                             /*!< ADC 0 Sample Sequence Length Register                                 */
    
    struct {
      __IO uint32_t  SL         :  4;               /*!< ADC 0 Sample Sequence Length Register                                 */
    } SL0_b;                                        /*!< BitSize                                                               */
  };
  
  union {
    __I  uint32_t  CU0;                             /*!< ADC 0 Sample Sequence Counter Register                                */
    
    struct {
      __I  uint32_t  CU         :  4;               /*!< ADC 0 Sample Sequence Counter Register. When completion of a
                                                         conversion, The CU will be to add one.                                */
    } CU0_b;                                        /*!< BitSize                                                               */
  };
  
  union {
    __I  uint32_t  A0RS0;                           /*!< ADC 0 Sample Result Register                                          */
    
    struct {
      __I  uint32_t  RS         : 12;               /*!< ADC 0 Sample Result Register. When the x-th conversion, the
                                                         end of conversion the result of the converted data is stored
                                                          in the registers.                                                    */
    } A0RS0_b;                                      /*!< BitSize                                                               */
  };
  
  union {
    __I  uint32_t  A0RS1;                           /*!< ADC 0 Sample Result Register                                          */
    
    struct {
      __I  uint32_t  RS         : 12;               /*!< ADC 0 Sample Result Register. When the x-th conversion, the
                                                         end of conversion the result of the converted data is stored
                                                          in the registers.                                                    */
    } A0RS1_b;                                      /*!< BitSize                                                               */
  };
  
  union {
    __I  uint32_t  A0RS2;                           /*!< ADC 0 Sample Result Register                                          */
    
    struct {
      __I  uint32_t  RS         : 12;               /*!< ADC 0 Sample Result Register. When the x-th conversion, the
                                                         end of conversion the result of the converted data is stored
                                                          in the registers.                                                    */
    } A0RS2_b;                                      /*!< BitSize                                                               */
  };
  
  union {
    __I  uint32_t  A0RS3;                           /*!< ADC 0 Sample Result Register                                          */
    
    struct {
      __I  uint32_t  RS         : 12;               /*!< ADC 0 Sample Result Register. When the x-th conversion, the
                                                         end of conversion the result of the converted data is stored
                                                          in the registers.                                                    */
    } A0RS3_b;                                      /*!< BitSize                                                               */
  };
  
  union {
    __I  uint32_t  A0RS4;                           /*!< ADC 0 Sample Result Register                                          */
    
    struct {
      __I  uint32_t  RS         : 12;               /*!< ADC 0 Sample Result Register. When the x-th conversion, the
                                                         end of conversion the result of the converted data is stored
                                                          in the registers.                                                    */
    } A0RS4_b;                                      /*!< BitSize                                                               */
  };
  
  union {
    __I  uint32_t  A0RS5;                           /*!< ADC 0 Sample Result Register                                          */
    
    struct {
      __I  uint32_t  RS         : 12;               /*!< ADC 0 Sample Result Register. When the x-th conversion, the
                                                         end of conversion the result of the converted data is stored
                                                          in the registers.                                                    */
    } A0RS5_b;                                      /*!< BitSize                                                               */
  };
  
  union {
    __I  uint32_t  A0RS6;                           /*!< ADC 0 Sample Result Register                                          */
    
    struct {
      __I  uint32_t  RS         : 12;               /*!< ADC 0 Sample Result Register. When the x-th conversion, the
                                                         end of conversion the result of the converted data is stored
                                                          in the registers.                                                    */
    } A0RS6_b;                                      /*!< BitSize                                                               */
  };
  
  union {
    __I  uint32_t  A0RS7;                           /*!< ADC 0 Sample Result Register                                          */
    
    struct {
      __I  uint32_t  RS         : 12;               /*!< ADC 0 Sample Result Register. When the x-th conversion, the
                                                         end of conversion the result of the converted data is stored
                                                          in the registers.                                                    */
    } A0RS7_b;                                      /*!< BitSize                                                               */
  };
  
  union {
    __IO uint32_t  SH0;                             /*!< ADC 0 Sample And Hold Register                                        */
    
    struct {
      __IO uint32_t  SH         :  8;               /*!< ADC 0 sample and hold time register. This register for CON[2].        */
    } SH0_b;                                        /*!< BitSize                                                               */
  };
  __I  uint32_t  RESERVED0[14];
  
  union {
    __O  uint32_t  IEENS;                           /*!< ADC Interrupt Enable Set Register                                     */
    
    struct {
      __O  uint32_t  SIN0       :  1;               /*!< When set, completion of a conversion on ADC 0.                        */
      __O  uint32_t  SEQ0       :  1;               /*!< When set, completion of a conversion on sequence 0.                   */
    } IEENS_b;                                      /*!< BitSize                                                               */
  };
  
  union {
    __O  uint32_t  IEENC;                           /*!< ADC Interrupt Enable Clear Register                                   */
    
    struct {
      __O  uint32_t  SIN0       :  1;               /*!< Writing a 1 will clear the bit corresponging to IEENS.                */
      __O  uint32_t  SEQ0       :  1;               /*!< Writing a 1 will clear the bit corresponging to IEENS.                */
    } IEENC_b;                                      /*!< BitSize                                                               */
  };
  
  union {
    __I  uint32_t  IEST;                            /*!< ADC Interrupt Status Register                                         */
    
    struct {
      __I  uint32_t  SIN0       :  1;               /*!< When set, completion of a conversion on ADC 0.                        */
      __I  uint32_t  SEQ0       :  1;               /*!< When set, completion of a conversion on sequence 0.                   */
    } IEST_b;                                       /*!< BitSize                                                               */
  };
  
  union {
    __O  uint32_t  IESTC;                           /*!< ADC Interrupt Status Clear Register                                   */
    
    struct {
      __O  uint32_t  SIN0       :  1;               /*!< Writing a 1 will clear the bit corresponging to IEST.                 */
      __O  uint32_t  SEQ0       :  1;               /*!< Writing a 1 will clear the bit corresponging to IEST.                 */
    } IESTC_b;                                      /*!< BitSize                                                               */
  };
  
  union {
    __I  uint32_t  RSCU0;                           /*!< ADC 0 RSIN Counter Register                                           */
    
    struct {
      __I  uint32_t  RSCU       :  4;               /*!< ADC 0 RSIN Counter Register                                           */
    } RSCU0_b;                                      /*!< BitSize                                                               */
  };
  __I  uint32_t  RESERVED1;
  
  union {
    __IO uint32_t  RSIN0;                           /*!< ADC 0 Result Index Register                                           */
    
    struct {
      __IO uint32_t  RSIN       :  4;               /*!< ADC 0 Result Index Register.                                          */
           uint32_t             : 27;
      __IO uint32_t  MODE       :  1;               /*!< The Index Mode.                                                       */
    } RSIN0_b;                                      /*!< BitSize                                                               */
  };
  __I  uint32_t  RESERVED2;
  
  union {
    __IO uint32_t  CD0;                             /*!< ADC 0 Clock Divider Register                                          */
    
    struct {
      __IO uint32_t  CD0        :  3;               /*!< ADC 0 Clock = System clock / (ADC0CD + 1)                             */
    } CD0_b;                                        /*!< BitSize                                                               */
  };
  __I  uint32_t  RESERVED3[3];
  
  union {
    __IO uint32_t  CALCON0;                         /*!< ADC 0 Calibration Control Register                                    */
    
    struct {
      __IO uint32_t  EN         :  1;               /*!< ADC Calibration Enable bit                                            */
      __IO uint32_t  BC         :  1;               /*!< BC SEL                                                                */
      __IO uint32_t  ST         :  1;               /*!< ST SEL                                                                */
           uint32_t             :  5;
      __IO uint32_t  START      :  1;               /*!< ADC Calibration Start bit                                             */
    } CALCON0_b;                                    /*!< BitSize                                                               */
  };
  
  union {
    __IO uint32_t  CALSTA0;                         /*!< ADC 0 Calibration Status Register                                     */
    
    struct {
      __IO uint32_t  STA        :  1;               /*!< Calibration status flag                                               */
      __IO uint32_t  GAIN       :  2;               /*!< Calibration gain control                                              */
      __IO uint32_t  TRIM       :  5;               /*!< Split capacitor trim                                                  */
    } CALSTA0_b;                                    /*!< BitSize                                                               */
  };
  
  union {
    __IO uint32_t  A0CALTOL0;                       /*!< ADC 0 Calibration Tolerances Register x                               */
    
    struct {
      __IO uint32_t  CALTOL     :  8;               /*!< Calibration Tolerances                                                */
    } A0CALTOL0_b;                                  /*!< BitSize                                                               */
  };
  
  union {
    __IO uint32_t  A0CALTOL1;                       /*!< ADC 0 Calibration Tolerances Register x                               */
    
    struct {
      __IO uint32_t  CALTOL     :  8;               /*!< Calibration Tolerances                                                */
    } A0CALTOL1_b;                                  /*!< BitSize                                                               */
  };
  
  union {
    __IO uint32_t  A0CALTOL2;                       /*!< ADC 0 Calibration Tolerances Register x                               */
    
    struct {
      __IO uint32_t  CALTOL     :  8;               /*!< Calibration Tolerances                                                */
    } A0CALTOL2_b;                                  /*!< BitSize                                                               */
  };
  
  union {
    __IO uint32_t  A0CALTOL3;                       /*!< ADC 0 Calibration Tolerances Register x                               */
    
    struct {
      __IO uint32_t  CALTOL     :  8;               /*!< Calibration Tolerances                                                */
    } A0CALTOL3_b;                                  /*!< BitSize                                                               */
  };
  
  union {
    __IO uint32_t  A0CALTOL4;                       /*!< ADC 0 Calibration Tolerances Register x                               */
    
    struct {
      __IO uint32_t  CALTOL     :  8;               /*!< Calibration Tolerances                                                */
    } A0CALTOL4_b;                                  /*!< BitSize                                                               */
  };
  
  union {
    __IO uint32_t  A0CALTOL5;                       /*!< ADC 0 Calibration Tolerances Register x                               */
    
    struct {
      __IO uint32_t  CALTOL     :  8;               /*!< Calibration Tolerances                                                */
    } A0CALTOL5_b;                                  /*!< BitSize                                                               */
  };
  __I  uint32_t  RESERVED4[8];
  
  union {
    __IO uint32_t  CONDMA0;                         /*!< ADC x Control DMA Register                                            */
    
    struct {
           uint32_t             : 28;
      __IO uint32_t  DMARREQ    :  1;               /*!< DMA Receiver Request. (Read only)                                     */
           uint32_t             :  2;
      __IO uint32_t  DMAE       :  1;               /*!< ADC x DMA Enable.                                                     */
    } CONDMA0_b;                                    /*!< BitSize                                                               */
  };
} OB_ADC_Type;


/* ================================================================================ */
/* ================                       PWM                      ================ */
/* ================================================================================ */


/**
  * @brief Power Control PWM Module (PWM)
  */

typedef struct {                                    /*!< PWM Structure                                                         */
  
  union {
    __IO uint32_t  CON;                             /*!< PWM Control Register                                                  */
    
    struct {
      __IO uint32_t  EN0        :  1;               /*!< PWM counter 0 Enable bit                                              */
      __IO uint32_t  MODE0      :  2;               /*!< PWM counter 0 active Mode Select bits                                 */
      __IO uint32_t  RSTCU0     :  1;               /*!< PWM counter 0 Reset bits                                              */
      __IO uint32_t  RSTCYC0    :  1;               /*!< PWM Cycle counter 0 Reset bits                                        */
      __IO uint32_t  RSTDEL0    :  1;               /*!< PWM Delay counter 0 Reset bits                                        */
           uint32_t             :  2;
      __IO uint32_t  EN1        :  1;               /*!< PWM counter 1 Enable bit                                              */
      __IO uint32_t  MODE1      :  2;               /*!< PWM counter 1 active Mode Select bits                                 */
      __IO uint32_t  RSTCU1     :  1;               /*!< PWM counter 1 Reset bits                                              */
      __IO uint32_t  RSTCYC1    :  1;               /*!< PWM Cycle counter 1 Reset bits                                        */
      __IO uint32_t  RSTDEL1    :  1;               /*!< PWM Delay counter 1 Reset bits                                        */
    } CON_b;                                        /*!< BitSize                                                               */
  };
  
  union {
    __IO uint32_t  MAX0;                            /*!< PWM Maximum Compare Register                                          */
    
    struct {
      __IO uint32_t  MAX        : 24;               /*!< PWM Maximum Compare Register.                                         */
    } MAX0_b;                                       /*!< BitSize                                                               */
  };
  
  union {
    __IO uint32_t  MAX1;                            /*!< PWM Maximum Compare Register                                          */
    
    struct {
      __IO uint32_t  MAX        : 24;               /*!< PWM Maximum Compare Register.                                         */
    } MAX1_b;                                       /*!< BitSize                                                               */
  };
  
  union {
    __IO uint32_t  CS;                              /*!< PWM Counter Selection                                                 */
    
    struct {
      __IO uint32_t  CMP0       :  1;               /*!< Select the Compare 0 source                                           */
      __IO uint32_t  CMP1       :  1;               /*!< Select the Compare 1 source                                           */
      __IO uint32_t  CMP2       :  1;               /*!< Select the Compare 2 source                                           */
      __IO uint32_t  CMP3       :  1;               /*!< Select the Compare 3 source                                           */
    } CS_b;                                         /*!< BitSize                                                               */
  };
  
  union {
    __IO uint32_t  OUTEN;                           /*!< PWM Output Enable Register                                            */
    
    struct {
      __IO uint32_t  PWM0A      :  1;               /*!< PWM0A Output Enable                                                   */
      __IO uint32_t  PWM0B      :  1;               /*!< PWM0B Output Enable                                                   */
      __IO uint32_t  PWM1A      :  1;               /*!< PWM1A Output Enable                                                   */
      __IO uint32_t  PWM1B      :  1;               /*!< PWM1B Output Enable                                                   */
      __IO uint32_t  PWM2A      :  1;               /*!< PWM2A Output Enable                                                   */
      __IO uint32_t  PWM2B      :  1;               /*!< PWM2B Output Enable                                                   */
    } OUTEN_b;                                      /*!< BitSize                                                               */
  };
  
  union {
    __IO uint32_t  OUTINI;                          /*!< PWM Output Initial Register                                           */
    
    struct {
      __IO uint32_t  PWM0A      :  1;               /*!< PWM0A Output Initial                                                  */
      __IO uint32_t  PWM0B      :  1;               /*!< PWM0B Output Initial                                                  */
      __IO uint32_t  PWM1A      :  1;               /*!< PWM1A Output Initial                                                  */
      __IO uint32_t  PWM1B      :  1;               /*!< PWM1B Output Initial                                                  */
      __IO uint32_t  PWM2A      :  1;               /*!< PWM2A Output Initial                                                  */
      __IO uint32_t  PWM2B      :  1;               /*!< PWM2B Output Initial                                                  */
    } OUTINI_b;                                     /*!< BitSize                                                               */
  };
  
  union {
    __IO uint32_t  OUTCON0A;                        /*!< PWM 0A Output Control Register                                        */
    
    struct {
      __IO uint32_t  ZERO0      :  2;               /*!< When the counter 0 is zero.                                           */
      __IO uint32_t  ZERO1      :  2;               /*!< When the counter 1 is zero.                                           */
      __IO uint32_t  MAX0       :  2;               /*!< When the counter 0 matches register MAX0.                             */
      __IO uint32_t  MAX1       :  2;               /*!< When the counter 1 matches register MAX1.                             */
      __IO uint32_t  DOWNC0     :  2;               /*!< When the counter matches comparator 0 While counting down.            */
      __IO uint32_t  UPC0       :  2;               /*!< When the counter matches comparator 0 While counting up.              */
      __IO uint32_t  DOWNC1     :  2;               /*!< When the counter matches comparator 1 While counting down.            */
      __IO uint32_t  UPC1       :  2;               /*!< When the counter matches comparator 1 While counting up.              */
      __IO uint32_t  DOWNC2     :  2;               /*!< When the counter matches comparator 2 While counting down.            */
      __IO uint32_t  UPC2       :  2;               /*!< When the counter matches comparator 2 While counting up.              */
      __IO uint32_t  DOWNC3     :  2;               /*!< When the counter matches comparator 3 While counting down.            */
      __IO uint32_t  UPC3       :  2;               /*!< When the counter matches comparator 3 While counting up.              */
    } OUTCON0A_b;                                   /*!< BitSize                                                               */
  };
  
  union {
    __IO uint32_t  OUTCON0B;                        /*!< PWM 0B Output Control Register                                        */
    
    struct {
      __IO uint32_t  ZERO0      :  2;               /*!< When the counter 0 is zero.                                           */
      __IO uint32_t  ZERO1      :  2;               /*!< When the counter 1 is zero.                                           */
      __IO uint32_t  MAX0       :  2;               /*!< When the counter 0 matches register MAX0.                             */
      __IO uint32_t  MAX1       :  2;               /*!< When the counter 1 matches register MAX1.                             */
      __IO uint32_t  DOWNC0     :  2;               /*!< When the counter matches comparator 0 While counting down.            */
      __IO uint32_t  UPC0       :  2;               /*!< When the counter matches comparator 0 While counting up.              */
      __IO uint32_t  DOWNC1     :  2;               /*!< When the counter matches comparator 1 While counting down.            */
      __IO uint32_t  UPC1       :  2;               /*!< When the counter matches comparator 1 While counting up.              */
      __IO uint32_t  DOWNC2     :  2;               /*!< When the counter matches comparator 2 While counting down.            */
      __IO uint32_t  UPC2       :  2;               /*!< When the counter matches comparator 2 While counting up.              */
      __IO uint32_t  DOWNC3     :  2;               /*!< When the counter matches comparator 3 While counting down.            */
      __IO uint32_t  UPC3       :  2;               /*!< When the counter matches comparator 3 While counting up.              */
    } OUTCON0B_b;                                   /*!< BitSize                                                               */
  };
  
  union {
    __IO uint32_t  OUTCON1A;                        /*!< PWM 1A Output Control Register                                        */
    
    struct {
      __IO uint32_t  ZERO0      :  2;               /*!< When the counter 0 is zero.                                           */
      __IO uint32_t  ZERO1      :  2;               /*!< When the counter 1 is zero.                                           */
      __IO uint32_t  MAX0       :  2;               /*!< When the counter 0 matches register MAX0.                             */
      __IO uint32_t  MAX1       :  2;               /*!< When the counter 1 matches register MAX1.                             */
      __IO uint32_t  DOWNC0     :  2;               /*!< When the counter matches comparator 0 While counting down.            */
      __IO uint32_t  UPC0       :  2;               /*!< When the counter matches comparator 0 While counting up.              */
      __IO uint32_t  DOWNC1     :  2;               /*!< When the counter matches comparator 1 While counting down.            */
      __IO uint32_t  UPC1       :  2;               /*!< When the counter matches comparator 1 While counting up.              */
      __IO uint32_t  DOWNC2     :  2;               /*!< When the counter matches comparator 2 While counting down.            */
      __IO uint32_t  UPC2       :  2;               /*!< When the counter matches comparator 2 While counting up.              */
      __IO uint32_t  DOWNC3     :  2;               /*!< When the counter matches comparator 3 While counting down.            */
      __IO uint32_t  UPC3       :  2;               /*!< When the counter matches comparator 3 While counting up.              */
    } OUTCON1A_b;                                   /*!< BitSize                                                               */
  };
  
  union {
    __IO uint32_t  OUTCON1B;                        /*!< PWM 1B Output Control Register                                        */
    
    struct {
      __IO uint32_t  ZERO0      :  2;               /*!< When the counter 0 is zero.                                           */
      __IO uint32_t  ZERO1      :  2;               /*!< When the counter 1 is zero.                                           */
      __IO uint32_t  MAX0       :  2;               /*!< When the counter 0 matches register MAX0.                             */
      __IO uint32_t  MAX1       :  2;               /*!< When the counter 1 matches register MAX1.                             */
      __IO uint32_t  DOWNC0     :  2;               /*!< When the counter matches comparator 0 While counting down.            */
      __IO uint32_t  UPC0       :  2;               /*!< When the counter matches comparator 0 While counting up.              */
      __IO uint32_t  DOWNC1     :  2;               /*!< When the counter matches comparator 1 While counting down.            */
      __IO uint32_t  UPC1       :  2;               /*!< When the counter matches comparator 1 While counting up.              */
      __IO uint32_t  DOWNC2     :  2;               /*!< When the counter matches comparator 2 While counting down.            */
      __IO uint32_t  UPC2       :  2;               /*!< When the counter matches comparator 2 While counting up.              */
      __IO uint32_t  DOWNC3     :  2;               /*!< When the counter matches comparator 3 While counting down.            */
      __IO uint32_t  UPC3       :  2;               /*!< When the counter matches comparator 3 While counting up.              */
    } OUTCON1B_b;                                   /*!< BitSize                                                               */
  };
  
  union {
    __IO uint32_t  OUTCON2A;                        /*!< PWM 2A Output Control Register                                        */
    
    struct {
      __IO uint32_t  ZERO0      :  2;               /*!< When the counter 0 is zero.                                           */
      __IO uint32_t  ZERO1      :  2;               /*!< When the counter 1 is zero.                                           */
      __IO uint32_t  MAX0       :  2;               /*!< When the counter 0 matches register MAX0.                             */
      __IO uint32_t  MAX1       :  2;               /*!< When the counter 1 matches register MAX1.                             */
      __IO uint32_t  DOWNC0     :  2;               /*!< When the counter matches comparator 0 While counting down.            */
      __IO uint32_t  UPC0       :  2;               /*!< When the counter matches comparator 0 While counting up.              */
      __IO uint32_t  DOWNC1     :  2;               /*!< When the counter matches comparator 1 While counting down.            */
      __IO uint32_t  UPC1       :  2;               /*!< When the counter matches comparator 1 While counting up.              */
      __IO uint32_t  DOWNC2     :  2;               /*!< When the counter matches comparator 2 While counting down.            */
      __IO uint32_t  UPC2       :  2;               /*!< When the counter matches comparator 2 While counting up.              */
      __IO uint32_t  DOWNC3     :  2;               /*!< When the counter matches comparator 3 While counting down.            */
      __IO uint32_t  UPC3       :  2;               /*!< When the counter matches comparator 3 While counting up.              */
    } OUTCON2A_b;                                   /*!< BitSize                                                               */
  };
  
  union {
    __IO uint32_t  OUTCON2B;                        /*!< PWM 2B Output Control Register                                        */
    
    struct {
      __IO uint32_t  ZERO0      :  2;               /*!< When the counter 0 is zero.                                           */
      __IO uint32_t  ZERO1      :  2;               /*!< When the counter 1 is zero.                                           */
      __IO uint32_t  MAX0       :  2;               /*!< When the counter 0 matches register MAX0.                             */
      __IO uint32_t  MAX1       :  2;               /*!< When the counter 1 matches register MAX1.                             */
      __IO uint32_t  DOWNC0     :  2;               /*!< When the counter matches comparator 0 While counting down.            */
      __IO uint32_t  UPC0       :  2;               /*!< When the counter matches comparator 0 While counting up.              */
      __IO uint32_t  DOWNC1     :  2;               /*!< When the counter matches comparator 1 While counting down.            */
      __IO uint32_t  UPC1       :  2;               /*!< When the counter matches comparator 1 While counting up.              */
      __IO uint32_t  DOWNC2     :  2;               /*!< When the counter matches comparator 2 While counting down.            */
      __IO uint32_t  UPC2       :  2;               /*!< When the counter matches comparator 2 While counting up.              */
      __IO uint32_t  DOWNC3     :  2;               /*!< When the counter matches comparator 3 While counting down.            */
      __IO uint32_t  UPC3       :  2;               /*!< When the counter matches comparator 3 While counting up.              */
    } OUTCON2B_b;                                   /*!< BitSize                                                               */
  };
  __I  uint32_t  RESERVED0[6];
  
  union {
    __IO uint32_t  DB0;                             /*!< Dead Band Register                                                    */
    
    struct {
      __IO uint32_t  DBN        : 16;               /*!< The DB[15:0] register contains the number of clock tick to delay
                                                         the PWM_XA and PWM_XB signal.                                         */
      __IO uint32_t  DBAE       :  1;               /*!< PWM_XA Edge Mode.The DB[15:0] register to delay the edge of
                                                         the PWM_XA signal when generating the PWM_XA signal.When DBN
                                                          = 0, This bit is ignored.                                            */
      __IO uint32_t  DBBE       :  1;               /*!< PWM_XB Edge ModeThe DB[15:0] register to delay the edge of the
                                                         PWM_XB signal when generating the PWM_XB signal.When DBN = 0,
                                                          This bit is ignored.                                                 */
    } DB0_b;                                        /*!< BitSize                                                               */
  };
  
  union {
    __IO uint32_t  DB1;                             /*!< Dead Band Register                                                    */
    
    struct {
      __IO uint32_t  DBN        : 16;               /*!< The DB[15:0] register contains the number of clock tick to delay
                                                         the PWM_XA and PWM_XB signal.                                         */
      __IO uint32_t  DBAE       :  1;               /*!< PWM_XA Edge Mode.The DB[15:0] register to delay the edge of
                                                         the PWM_XA signal when generating the PWM_XA signal.When DBN
                                                          = 0, This bit is ignored.                                            */
      __IO uint32_t  DBBE       :  1;               /*!< PWM_XB Edge ModeThe DB[15:0] register to delay the edge of the
                                                         PWM_XB signal when generating the PWM_XB signal.When DBN = 0,
                                                          This bit is ignored.                                                 */
    } DB1_b;                                        /*!< BitSize                                                               */
  };
  
  union {
    __IO uint32_t  DB2;                             /*!< Dead Band Register                                                    */
    
    struct {
      __IO uint32_t  DBN        : 16;               /*!< The DB[15:0] register contains the number of clock tick to delay
                                                         the PWM_XA and PWM_XB signal.                                         */
      __IO uint32_t  DBAE       :  1;               /*!< PWM_XA Edge Mode.The DB[15:0] register to delay the edge of
                                                         the PWM_XA signal when generating the PWM_XA signal.When DBN
                                                          = 0, This bit is ignored.                                            */
      __IO uint32_t  DBBE       :  1;               /*!< PWM_XB Edge ModeThe DB[15:0] register to delay the edge of the
                                                         PWM_XB signal when generating the PWM_XB signal.When DBN = 0,
                                                          This bit is ignored.                                                 */
    } DB2_b;                                        /*!< BitSize                                                               */
  };
  __I  uint32_t  RESERVED1[3];
  
  union {
    __IO uint32_t  CMP0;                            /*!< PWM Compare Register                                                  */
    
    struct {
      __IO uint32_t  CMP        : 24;               /*!< PWM Compare Register.                                                 */
    } CMP0_b;                                       /*!< BitSize                                                               */
  };
  
  union {
    __IO uint32_t  CMP1;                            /*!< PWM Compare Register                                                  */
    
    struct {
      __IO uint32_t  CMP        : 24;               /*!< PWM Compare Register.                                                 */
    } CMP1_b;                                       /*!< BitSize                                                               */
  };
  
  union {
    __IO uint32_t  CMP2;                            /*!< PWM Compare Register                                                  */
    
    struct {
      __IO uint32_t  CMP        : 24;               /*!< PWM Compare Register.                                                 */
    } CMP2_b;                                       /*!< BitSize                                                               */
  };
  
  union {
    __IO uint32_t  CMP3;                            /*!< PWM Compare Register                                                  */
    
    struct {
      __IO uint32_t  CMP        : 24;               /*!< PWM Compare Register.                                                 */
    } CMP3_b;                                       /*!< BitSize                                                               */
  };
  __I  uint32_t  RESERVED2[2];
  
  union {
    __I  uint32_t  CU0;                             /*!< Current Counter                                                       */
    
    struct {
      __I  uint32_t  CU         : 24;               /*!< Current Counter                                                       */
    } CU0_b;                                        /*!< BitSize                                                               */
  };
  
  union {
    __I  uint32_t  CU1;                             /*!< Current Counter                                                       */
    
    struct {
      __I  uint32_t  CU         : 24;               /*!< Current Counter                                                       */
    } CU1_b;                                        /*!< BitSize                                                               */
  };
  
  union {
    __IO uint32_t  CYCMP0;                          /*!< PWM Cycle Compare Register                                            */
    
    struct {
      __IO uint32_t  CYCCMP     : 16;               /*!< PWM Cycle Compare Register                                            */
    } CYCMP0_b;                                     /*!< BitSize                                                               */
  };
  
  union {
    __IO uint32_t  CYCMP1;                          /*!< PWM Cycle Compare Register                                            */
    
    struct {
      __IO uint32_t  CYCCMP     : 16;               /*!< PWM Cycle Compare Register                                            */
    } CYCMP1_b;                                     /*!< BitSize                                                               */
  };
  
  union {
    __I  uint32_t  CYCU0;                           /*!< PWM Cycle Counter Register                                            */
    
    struct {
      __I  uint32_t  CYCU       : 16;               /*!< Cycle Counter Register.                                               */
    } CYCU0_b;                                      /*!< BitSize                                                               */
  };
  
  union {
    __I  uint32_t  CYCU1;                           /*!< PWM Cycle Counter Register                                            */
    
    struct {
      __I  uint32_t  CYCU       : 16;               /*!< Cycle Counter Register.                                               */
    } CYCU1_b;                                      /*!< BitSize                                                               */
  };
  
  union {
    __IO uint32_t  ADCTDEL;                         /*!< PWM ADC Trigger Delay Register                                        */
    
    struct {
      __IO uint32_t  ADCTDEL    : 24;               /*!< ADC Trigger Delay Register.                                           */
    } ADCTDEL_b;                                    /*!< BitSize                                                               */
  };
  __I  uint32_t  RESERVED3;
  
  union {
    __I  uint32_t  DELCU0;                          /*!< PWM Delay Counter Register                                            */
    
    struct {
      __I  uint32_t  DELCU      : 24;               /*!< ADC0 Sample Result 0 Register. When 1st conversion, the end
                                                         of conversion the result of the converted data is stored in
                                                          the registers.                                                       */
    } DELCU0_b;                                     /*!< BitSize                                                               */
  };
  
  union {
    __I  uint32_t  DELCU1;                          /*!< PWM Delay Counter Register                                            */
    
    struct {
      __I  uint32_t  DELCU      : 24;               /*!< ADC0 Sample Result 0 Register. When 1st conversion, the end
                                                         of conversion the result of the converted data is stored in
                                                          the registers.                                                       */
    } DELCU1_b;                                     /*!< BitSize                                                               */
  };
  
  union {
    __IO uint32_t  IEENS;                           /*!< PWM Interrupt Enable Set Register                                     */
    
    struct {
      __IO uint32_t  ZERO0      :  1;               /*!< When following conditions are true: 1. The cycle counter has
                                                         matched cycle comparator.2. The counter 0 has matched 0.              */
      __IO uint32_t  ZERO1      :  1;               /*!< When following conditions are true: 1. The cycle counter has
                                                         matched cycle comparator.2. The counter 1 has matched 0.              */
      __IO uint32_t  MAX0       :  1;               /*!< When following conditions are true: 1. The cycle counter has
                                                         matched cycle comparator.2. The counter 0 has matched MAX0 register.  */
      __IO uint32_t  MAX1       :  1;               /*!< When following conditions are true: 1. The cycle counter has
                                                         matched cycle comparator.2. The counter 1 has matched MAX1 register.  */
      __IO uint32_t  DOWMC0     :  1;               /*!< When following conditions are true: 1. The cycle counter has
                                                         matched cycle comparator.2. The counter 0/counter 1 has matched
                                                          comparator 0 while counting down.                                    */
      __IO uint32_t  UPC0       :  1;               /*!< When following conditions are true: 1. The cycle counter has
                                                         matched cycle comparator.2. The counter 0/counter 1 has matched
                                                          comparator 0 while counting up.                                      */
      __IO uint32_t  DOWMC1     :  1;               /*!< When following conditions are true: 1. The cycle counter has
                                                         matched cycle comparator.2. The counter 0/counter 1 has matched
                                                          comparator 1 while counting down.                                    */
      __IO uint32_t  UPC1       :  1;               /*!< When following conditions are true: 1. The cycle counter has
                                                         matched cycle comparator.2. The counter 0/counter 1 has matched
                                                          comparator 1 while counting up.                                      */
      __IO uint32_t  DOWMC2     :  1;               /*!< When following conditions are true: 1. The cycle counter has
                                                         matched cycle comparator.2.The counter 0/counter 1 has matched
                                                          comparator 2 while counting down.                                    */
      __IO uint32_t  UPC2       :  1;               /*!< When following conditions are true: 1. The cycle counter has
                                                         matched cycle comparator.2. The counter 0/counter 1 has matched
                                                          comparator 2 while counting up.                                      */
      __IO uint32_t  DOWMC3     :  1;               /*!< When following conditions are true: 1. The cycle counter has
                                                         matched cycle comparator.2. The counter 0/counter 1 has matched
                                                          comparator 3 while counting down.                                    */
      __IO uint32_t  UPC3       :  1;               /*!< When following conditions are true: 1. The cycle counter has
                                                         matched cycle comparator.2. The counter 0/counter 1 has matched
                                                          comparator 3 while counting up.                                      */
           uint32_t             :  4;
      __IO uint32_t  FAULTA     :  1;               /*!< When the FAULT A input is asserted.                                   */
      __IO uint32_t  FAULTB     :  1;               /*!< When the FAULT B input is asserted.                                   */
      __IO uint32_t  FAULTC     :  1;               /*!< When the FAULT C input is asserted.                                   */
      __IO uint32_t  FAULTD     :  1;               /*!< When the FAULT D condition is generation.                             */
    } IEENS_b;                                      /*!< BitSize                                                               */
  };
  
  union {
    __IO uint32_t  IEENC;                           /*!< PWM Interrupt Enable Clear Register                                   */
    
    struct {
      __IO uint32_t  ZERO0      :  1;               /*!< Writing a 1 will clear the interrupt corresponding to IEENS.          */
      __IO uint32_t  ZERO1      :  1;               /*!< Writing a 1 will clear the interrupt corresponding to IEENS.          */
      __IO uint32_t  MAX0       :  1;               /*!< Writing a 1 will clear the interrupt corresponding to IEENS.          */
      __IO uint32_t  MAX1       :  1;               /*!< Writing a 1 will clear the interrupt corresponding to IEENS.          */
      __IO uint32_t  DOWMC0     :  1;               /*!< Writing a 1 will clear the interrupt corresponding to IEENS.          */
      __IO uint32_t  UPC0       :  1;               /*!< Writing a 1 will clear the interrupt corresponding to IEENS.          */
      __IO uint32_t  DOWMC1     :  1;               /*!< Writing a 1 will clear the interrupt corresponding to IEENS.          */
      __IO uint32_t  UPC1       :  1;               /*!< Writing a 1 will clear the interrupt corresponding to IEENS.          */
      __IO uint32_t  DOWMC2     :  1;               /*!< Writing a 1 will clear the interrupt corresponding to IEENS.          */
      __IO uint32_t  UPC2       :  1;               /*!< Writing a 1 will clear the interrupt corresponding to IEENS.          */
      __IO uint32_t  DOWMC3     :  1;               /*!< Writing a 1 will clear the interrupt corresponding to IEENS.          */
      __IO uint32_t  UPC3       :  1;               /*!< Writing a 1 will clear the interrupt corresponding to IEENS.          */
           uint32_t             :  4;
      __IO uint32_t  FAULTA     :  1;               /*!< Writing a 1 will clear the interrupt corresponding to IEENS.          */
      __IO uint32_t  FAULTB     :  1;               /*!< Writing a 1 will clear the interrupt corresponding to IEENS.          */
      __IO uint32_t  FAULTC     :  1;               /*!< Writing a 1 will clear the interrupt corresponding to IEENS.          */
      __IO uint32_t  FAULTD     :  1;               /*!< Writing a 1 will clear the interrupt corresponding to IEENS.          */
    } IEENC_b;                                      /*!< BitSize                                                               */
  };
  
  union {
    __I  uint32_t  EVST;                            /*!< PWM Event Status Register                                             */
    
    struct {
      __I  uint32_t  ZERO0      :  1;               /*!< When following conditions are true: 1. The cycle counter has
                                                         matched cycle comparator.2. The counter 0 has matched 0.              */
      __I  uint32_t  ZERO1      :  1;               /*!< When following conditions are true: 1. The cycle counter has
                                                         matched cycle comparator.2. The counter 1 has matched 0.              */
      __I  uint32_t  MAX0       :  1;               /*!< When following conditions are true: 1. The cycle counter has
                                                         matched cycle comparator.2.The counter 0 has matched MAX0 register.   */
      __I  uint32_t  MAX1       :  1;               /*!< When following conditions are true: 1. The cycle counter has
                                                         matched cycle comparator.2. The counter 1 has matched MAX1 register.  */
      __I  uint32_t  DOWMC0     :  1;               /*!< When following conditions are true: 1. The cycle counter has
                                                         matched cycle comparator.2. The counter 0 / counter 1 has matched
                                                          comparator 0 while counting down.                                    */
      __I  uint32_t  UPC0       :  1;               /*!< When following conditions are true: 1. The cycle counter has
                                                         matched cycle comparator.2. The counter 0/ counter 1 has matched
                                                          comparator 0 while counting up.                                      */
      __I  uint32_t  DOWMC1     :  1;               /*!< When following conditions are true: 1. The cycle counter has
                                                         matched cycle comparator.2. The counter 0 / counter 1 has matched
                                                          comparator 1 while counting down.                                    */
      __I  uint32_t  UPC1       :  1;               /*!< When following conditions are true: 1. The cycle counter has
                                                         matched cycle comparator.2. The counter 0/ counter 1 has matched
                                                          comparator 1 while counting up.                                      */
      __I  uint32_t  DOWMC2     :  1;               /*!< When following conditions are true: 1. The cycle counter has
                                                         matched cycle comparator.2. The counter 0 / counter 1 has matched
                                                          comparator 2 while counting down.                                    */
      __I  uint32_t  UPC2       :  1;               /*!< When following conditions are true: 1. The cycle counter has
                                                         matched cycle comparator.2. The counter 0/ counter 1 has matched
                                                          comparator 2 while counting up.                                      */
      __I  uint32_t  DOWMC3     :  1;               /*!< When following conditions are true: 1. The cycle counter has
                                                         matched cycle comparator.2. The counter 0 / counter 1 has matched
                                                          comparator 3 while counting down.                                    */
      __I  uint32_t  UPC3       :  1;               /*!< When following conditions are true: 1. The cycle counter has
                                                         matched cycle comparator.2. The counter 0/ counter 1 has matched
                                                          comparator 3 while counting up.                                      */
           uint32_t             :  4;
      __I  uint32_t  FAULTA     :  1;               /*!< When the FAULT A input is asserted.                                   */
      __I  uint32_t  FAULTB     :  1;               /*!< When the FAULT B input is asserted.                                   */
      __I  uint32_t  FAULTC     :  1;               /*!< When the FAULT C input is asserted.                                   */
      __I  uint32_t  FAULTD     :  1;               /*!< When the FAULT D condition is generation.                             */
    } EVST_b;                                       /*!< BitSize                                                               */
  };
  
  union {
    __IO uint32_t  EVSTC;                           /*!< PWM Event Status Clear Register                                       */
    
    struct {
      __IO uint32_t  ZERO0      :  1;               /*!< Writing a 1 will clear the event status bit corresponding to
                                                         EVST.                                                                 */
      __IO uint32_t  ZERO1      :  1;               /*!< Writing a 1 will clear the event status bit corresponding to
                                                         EVST.                                                                 */
      __IO uint32_t  MAX0       :  1;               /*!< Writing a 1 will clear the event status bit corresponding to
                                                         EVST.                                                                 */
      __IO uint32_t  MAX1       :  1;               /*!< Writing a 1 will clear the event status bit corresponding to
                                                         EVST.                                                                 */
      __IO uint32_t  DOWMC0     :  1;               /*!< Writing a 1 will clear the event status bit corresponding to
                                                         EVST.                                                                 */
      __IO uint32_t  UPC0       :  1;               /*!< Writing a 1 will clear the event status bit corresponding to
                                                         EVST.                                                                 */
      __IO uint32_t  DOWMC1     :  1;               /*!< Writing a 1 will clear the event status bit corresponding to
                                                         EVST.                                                                 */
      __IO uint32_t  UPC1       :  1;               /*!< Writing a 1 will clear the event status bit corresponding to
                                                         EVST.                                                                 */
      __IO uint32_t  DOWMC2     :  1;               /*!< Writing a 1 will clear the event status bit corresponding to
                                                         EVST.                                                                 */
      __IO uint32_t  UPC2       :  1;               /*!< Writing a 1 will clear the event status bit corresponding to
                                                         EVST.                                                                 */
      __IO uint32_t  DOWMC3     :  1;               /*!< Writing a 1 will clear the event status bit corresponding to
                                                         EVST.                                                                 */
      __IO uint32_t  UPC3       :  1;               /*!< Writing a 1 will clear the event status bit corresponding to
                                                         EVST.                                                                 */
           uint32_t             :  4;
      __IO uint32_t  FAULTA     :  1;               /*!< Writing a 1 will clear the event status bit corresponding to
                                                         EVST.                                                                 */
      __IO uint32_t  FAULTB     :  1;               /*!< Writing a 1 will clear the event status bit corresponding to
                                                         EVST.                                                                 */
      __IO uint32_t  FAULTC     :  1;               /*!< Writing a 1 will clear the event status bit corresponding to
                                                         EVST.                                                                 */
      __IO uint32_t  FAULTD     :  1;               /*!< Writing a 1 will clear the event status bit corresponding to
                                                         EVST.                                                                 */
    } EVSTC_b;                                      /*!< BitSize                                                               */
  };
  
  union {
    __IO uint32_t  FLTCON;                          /*!< Fault Configure Register                                              */
    
    struct {
      __IO uint32_t  ENA        :  1;               /*!< Fault A Enable bit                                                    */
      __IO uint32_t  ACTA       :  1;               /*!< The event trigger Fault A                                             */
      __IO uint32_t  SSA0       :  1;               /*!< Select Fault A pin as the Fault A signal                              */
      __IO uint32_t  SSA1       :  1;               /*!< Select Comparator 0 as the Fault A signal                             */
      __IO uint32_t  SSA2       :  1;               /*!< Select Comparator 1 as the Fault A signal                             */
      __IO uint32_t  SSA3       :  1;               /*!< Select Comparator 2 as the Fault A signal                             */
      __IO uint32_t  SSA4       :  1;               /*!< Select Comparator 3 as the Fault A signal                             */
           uint32_t             :  1;
      __IO uint32_t  ENB        :  1;               /*!< Fault B Enable bit                                                    */
      __IO uint32_t  ACTB       :  1;               /*!< The event trigger Fault B                                             */
      __IO uint32_t  SSB0       :  1;               /*!< Select Fault B pin as the Fault B signal                              */
      __IO uint32_t  SSB1       :  1;               /*!< Select Comparator 0 as the Fault B signal                             */
      __IO uint32_t  SSB2       :  1;               /*!< Select Comparator 1 as the Fault B signal                             */
      __IO uint32_t  SSB3       :  1;               /*!< Select Comparator 2 as the Fault B signal                             */
      __IO uint32_t  SSB4       :  1;               /*!< Select Comparator 3 as the Fault B signal                             */
           uint32_t             :  1;
      __IO uint32_t  ENC        :  1;               /*!< Fault C Enable bit                                                    */
      __IO uint32_t  ACTC       :  1;               /*!< The event trigger Fault C                                             */
      __IO uint32_t  SSC0       :  1;               /*!< Select Fault C pin as the Fault C signal                              */
      __IO uint32_t  SSC1       :  1;               /*!< Select Comparator 0 as the Fault C signal                             */
      __IO uint32_t  SSC2       :  1;               /*!< Select Comparator 1 as the Fault C signal                             */
      __IO uint32_t  SSC3       :  1;               /*!< Select Comparator 2 as the Fault C signal                             */
      __IO uint32_t  SSC4       :  1;               /*!< Select Comparator 3 as the Fault C signal                             */
           uint32_t             :  1;
      __IO uint32_t  END0       :  1;               /*!< Fault D Enable bit (PWM0A and PWM0B)                                  */
      __IO uint32_t  END1       :  1;               /*!< Fault D Enable bit (PWM1A and PWM1B)                                  */
      __IO uint32_t  END2       :  1;               /*!< Fault D Enable bit (PWM2A and PWM2B)                                  */
      __IO uint32_t  END3       :  1;               /*!< Fault D Enable bit (PWM3A and PWM3B)                                  */
           uint32_t             :  2;
      __IO uint32_t  DLEVEL     :  2;               /*!< When following conditions are true, the Fault D occurs event.         */
    } FLTCON_b;                                     /*!< BitSize                                                               */
  };
  
  union {
    __IO uint32_t  FLTOTEN;                         /*!< Fault Output Enable Register                                          */
    
    struct {
      __IO uint32_t  PWM0A      :  1;               /*!< The PWM0A output signal on a Fault condition.                         */
      __IO uint32_t  PWM0B      :  1;               /*!< The PWM0B output signal on a Fault condition.                         */
      __IO uint32_t  PWM1A      :  1;               /*!< The PWM1A output signal on a Fault condition.                         */
      __IO uint32_t  PWM1B      :  1;               /*!< The PWM1B output signal on a Fault condition.                         */
      __IO uint32_t  PWM2A      :  1;               /*!< The PWM2A output signal on a Fault condition.                         */
      __IO uint32_t  PWM2B      :  1;               /*!< The PWM2B output signal on a Fault condition.                         */
    } FLTOTEN_b;                                    /*!< BitSize                                                               */
  };
  
  union {
    __IO uint32_t  FLTOL;                           /*!< Fault Output Level Register                                           */
    
    struct {
      __IO uint32_t  PWM0A      :  1;               /*!< The PWM0A output signal on a Fault condition.                         */
      __IO uint32_t  PWM0B      :  1;               /*!< The PWM0B output signal on a Fault condition.                         */
      __IO uint32_t  PWM1A      :  1;               /*!< The PWM1A output signal on a Fault condition.                         */
      __IO uint32_t  PWM1B      :  1;               /*!< The PWM1B output signal on a Fault condition.                         */
      __IO uint32_t  PWM2A      :  1;               /*!< The PWM2A output signal on a Fault condition.                         */
      __IO uint32_t  PWM2B      :  1;               /*!< The PWM2B output signal on a Fault condition.                         */
    } FLTOL_b;                                      /*!< BitSize                                                               */
  };
  
  union {
    __IO uint32_t  FLTNFA;                          /*!< PWM Fault A Noise Filter Register                                     */
    
    struct {
      __IO uint32_t  FLTNFA     : 16;               /*!< Fault A noise filter.                                                 */
    } FLTNFA_b;                                     /*!< BitSize                                                               */
  };
  
  union {
    __IO uint32_t  FLTNFB;                          /*!< PWM Fault B Noise Filter Register                                     */
    
    struct {
      __IO uint32_t  FLTNFB     : 16;               /*!< Fault B noise filter.                                                 */
    } FLTNFB_b;                                     /*!< BitSize                                                               */
  };
  
  union {
    __IO uint32_t  FLTNFC;                          /*!< PWM Fault C Noise Filter Register                                     */
    
    struct {
      __IO uint32_t  FLTNFC     : 16;               /*!< Fault C noise filter.                                                 */
    } FLTNFC_b;                                     /*!< BitSize                                                               */
  };
  
  union {
    __I  uint32_t  STA;                             /*!< PWM Status Register                                                   */
    
    struct {
      __I  uint32_t  DIR0       :  1;               /*!< PWM Current Count 0 Direction Status.                                 */
           uint32_t             :  7;
      __I  uint32_t  DIR1       :  1;               /*!< PWM Current Count 1 Direction Status.                                 */
    } STA_b;                                        /*!< BitSize                                                               */
  };
  
  union {
    __IO uint32_t  TADC0;                           /*!< PWM Trigger ADC Enable Register                                       */
    
    struct {
      __IO uint32_t  ZERO0      :  1;               /*!< When following conditions are true, PWM trigger ADC x to start
                                                         conversion: 1. The cycle counter has matched cycle comparator.
                                                          2. The ADCTDEL has matched DELCU. 3. The counter 0 has matched
                                                          0.                                                                   */
      __IO uint32_t  ZERO1      :  1;               /*!< When following conditions are true, PWM trigger ADC x to start
                                                         conversion: 1. The cycle counter has matched cycle comparator.
                                                          2. The ADCTDEL has matched DELCU. 3. The counter 1 has matched
                                                          0.                                                                   */
      __IO uint32_t  MAX0       :  1;               /*!< When following conditions are true, PWM trigger ADC x to start
                                                         conversion: 1. The cycle counter has matched cycle comparator.
                                                          2. The ADCTDEL has matched DELCU. 3. The counter 0 has matched
                                                          register MAX0.                                                       */
      __IO uint32_t  MAX1       :  1;               /*!< When following conditions are true, PWM trigger ADC x to start
                                                         conversion: 1. The cycle counter has matched cycle comparator.
                                                          2. The ADCTDEL has matched DELCU. 3. The counter 1 has matched
                                                          register MAX1.                                                       */
      __IO uint32_t  DOWMC0     :  1;               /*!< When following conditions are true, PWM trigger ADC x to start
                                                         conversion: 1. The cycle counter has matched cycle comparator.
                                                          2. The ADCTDEL has matched DELCU. 3. The counter 0/ counter
                                                          1 has matched comparator 0 While counting down.                      */
      __IO uint32_t  UPC0       :  1;               /*!< When following conditions are true, PWM trigger ADC x to start
                                                         conversion: 1. The cycle counter has matched cycle comparator.
                                                          2. The ADCTDEL has matched DELCU. 3. The counter 0/ counter
                                                          1 has matched comparator 0 While counting up.                        */
      __IO uint32_t  DOWMC1     :  1;               /*!< When following conditions are true, PWM trigger ADC x to start
                                                         conversion: 1.The cycle counter has matched cycle comparator.
                                                          2. The ADCTDEL has matched DELCU. 3.The counter 0/ counter 1
                                                          has matched comparator 1 While counting down.                        */
      __IO uint32_t  UPC1       :  1;               /*!< When following conditions are true, PWM trigger ADC x to start
                                                         conversion: 1. The cycle counter has matched cycle comparator.
                                                          2. The ADCTDEL has matched DELCU. 3. The counter 0/ counter
                                                          1 has matched comparator 1 While counting up.                        */
      __IO uint32_t  DOWMC2     :  1;               /*!< When following conditions are true, PWM trigger ADC x to start
                                                         conversion: 1. The cycle counter has matched cycle comparator.
                                                          2. The ADCTDEL has matched DELCU. 3. The counter 0/ counter
                                                          1 has matched comparator 2 While counting down.                      */
      __IO uint32_t  UPC2       :  1;               /*!< When following conditions are true, PWM trigger ADC x to start
                                                         conversion: 1. The cycle counter has matched cycle comparator.
                                                          2. The ADCTDEL has matched DELCU. 3. The counter 0/ counter
                                                          1 has matched comparator 2 While counting up.                        */
      __IO uint32_t  DOWMC3     :  1;               /*!< When following conditions are true, PWM trigger ADC x to start
                                                         conversion: 1. The cycle counter has matched cycle comparator.
                                                          2. The ADCTDEL has matched DELCU. 3. The counter 0/ counter
                                                          1 has matched comparator 3 While counting down.                      */
      __IO uint32_t  UPC3       :  1;               /*!< When following conditions are true, PWM trigger ADC x to start
                                                         conversion: 1. The cycle counter has matched cycle comparator.
                                                          2. The ADCTDEL has matched DELCU. 3. The counter 0/ counter
                                                          1 has matched comparator 3 While counting up.                        */
    } TADC0_b;                                      /*!< BitSize                                                               */
  };
  __I  uint32_t  RESERVED4;
  
  union {
    __IO uint32_t  UPDATE;                          /*!< PWM Update Mode Enable Register                                       */
    
    struct {
      __IO uint32_t  ZERO0      :  1;               /*!< When following conditions are true, update to the 'Double Buff'
                                                         registers: 1. The cycle counter has matched cycle comparator.
                                                          2. The counter 0 has matched 0.                                      */
      __IO uint32_t  ZERO1      :  1;               /*!< When following conditions are true, update to the 'Double Buff'
                                                         registers: 1. The cycle counter has matched cycle comparator.
                                                          2. The counter 1 has matched 0.                                      */
      __IO uint32_t  MAX0       :  1;               /*!< When following conditions are true, update to the 'Double Buff'
                                                         registers: 1. The cycle counter has matched cycle comparator.
                                                          2. The counter 0 has matched register MAX0.                          */
      __IO uint32_t  MAX1       :  1;               /*!< When following conditions are true, update to the 'Double Buff'
                                                         registers: 1. The cycle counter has matched cycle comparator.
                                                          2. The counter 1 has matched register MAX1.                          */
      __IO uint32_t  DOWMC0     :  1;               /*!< When following conditions are true, update to the 'Double Buff'
                                                         registers: 1. The cycle counter has matched cycle comparator.
                                                          2. The counter 0/ counter 1 has matched comparator 0 While counting
                                                          down.                                                                */
      __IO uint32_t  UPC0       :  1;               /*!< When following conditions are true, update to the 'Double Buff'
                                                         registers: 1. The cycle counter has matched cycle comparator.
                                                          2. The counter 0/ counter 1 has matched comparator 0 While counting
                                                          up.                                                                  */
      __IO uint32_t  DOWMC1     :  1;               /*!< When following conditions are true, update to the 'Double Buff'
                                                         registers: 1. The cycle counter has matched cycle comparator.
                                                          2. The counter 0/ counter 1 has matched comparator1 While counting
                                                          down.                                                                */
      __IO uint32_t  UPC1       :  1;               /*!< When following conditions are true, update to the 'Double Buff'
                                                         registers: 1. The cycle counter has matched cycle comparator.
                                                          2. The counter 0/ counter 1 has matched comparator 1 While counting
                                                          up.                                                                  */
      __IO uint32_t  DOWMC2     :  1;               /*!< When following conditions are true, update to the 'Double Buff'
                                                         registers: 1. The cycle counter has matched cycle comparator.
                                                          2. The counter 0/ counter 1 has matched comparator 2 While counting
                                                          down.                                                                */
      __IO uint32_t  UPC2       :  1;               /*!< When following conditions are true, update to the 'Double Buff'
                                                         registers: 1. The cycle counter has matched cycle comparator.
                                                          2. The counter 0/ counter 1 has matched comparator 2 While counting
                                                          up.                                                                  */
      __IO uint32_t  DOWMC3     :  1;               /*!< When following conditions are true, update to the 'Double Buff'
                                                         registers: 1. The cycle counter has matched cycle comparator.
                                                          2. The counter 0/ counter 1 has matched comparator 3 While counting
                                                          down.                                                                */
      __IO uint32_t  UPC3       :  1;               /*!< When following conditions are true, update to the 'Double Buff'
                                                         registers: 1. The cycle counter has matched cycle comparator.
                                                          2. The counter 0/ counter 1 has matched comparator 3 While counting
                                                          up.                                                                  */
           uint32_t             :  4;
      __IO uint32_t  IMMED      :  1;               /*!< Immediately                                                           */
    } UPDATE_b;                                     /*!< BitSize                                                               */
  };
  
  union {
    __IO uint32_t  FLTREL;                          /*!< PWM Fault Release Register                                            */
    
    struct {
      __IO uint32_t  ZERO0      :  1;               /*!< When following conditions are true, release the fault output:
                                                         1. The FLTST register = 0x00. 2. The counter 0 has matched 0.
                                                          (Ignore item1 and FLTST register is cleared to 0, when the FLTREL[31]
                                                          register is equal to 1)                                              */
      __IO uint32_t  ZERO1      :  1;               /*!< When following conditions are true, release the fault output:
                                                         1. The FLTST register = 0x00. 2. The counter 1 has matched 0.
                                                          (Ignore item1 and FLTST register is cleared to 0, when the FLTREL[31]
                                                          register is equal to 1)                                              */
      __IO uint32_t  MAX0       :  1;               /*!< When following conditions are true, release the fault output:
                                                         1. The FLTST register = 0x00. 2. The counter 0 has matched MAX0.
                                                          (Ignore item1 and FLTST register is cleared to 0, when the FLTREL[31]
                                                          register is equal to 1)                                              */
      __IO uint32_t  MAX1       :  1;               /*!< When following conditions are true, release the fault output:
                                                         1. The FLTST register = 0x00. 2. The counter 1 has matched MAX1.
                                                          (Ignore item1 and FLTST register is cleared to 0, when the FLTREL[31]
                                                          register is equal to 1)                                              */
      __IO uint32_t  DOWMC0     :  1;               /*!< When following conditions are true, release the fault output:
                                                         1. The FLTST register = 0x00. 2. The counter 0/ counter 1 has
                                                          matched comparator 0 While counting down. (Ignore item1 and
                                                          FLTST register is cleared to 0, when the FLTREL[31] register
                                                          is equal to 1)                                                       */
      __IO uint32_t  UPC0       :  1;               /*!< When following conditions are true, release the fault output:
                                                         1. The FLTST register = 0x00. 2. The counter 0/ counter 1 has
                                                          matched comparator 0 While counting up. (Ignore item1 and FLTST
                                                          register is cleared to 0, when the FLTREL[31] register is equal
                                                          to 1)                                                                */
      __IO uint32_t  DOWMC1     :  1;               /*!< When following conditions are true, release the fault output:
                                                         1. The FLTST register = 0x00. 2. The counter 0/ counter 1 has
                                                          matched comparator1 While counting down. (Ignore item1 and FLTST
                                                          register is cleared to 0, when the FLTREL[31] register is equal
                                                          to 1)                                                                */
      __IO uint32_t  UPC1       :  1;               /*!< When following conditions are true, release the fault output:
                                                         1. The FLTST register = 0x00. 2. The counter 0/ counter 1 has
                                                          matched comparator 1 While counting up. (Ignore item1 and FLTST
                                                          register is cleared to 0, when the FLTREL[31] register is equal
                                                          to 1)                                                                */
      __IO uint32_t  DOWMC2     :  1;               /*!< When following conditions are true, release the fault output:
                                                         1. The FLTST register = 0x00. 2. The counter 0/ counter 1 has
                                                          matched comparator 2 While counting down. (Ignore item1 and
                                                          FLTST register is cleared to 0, when the FLTREL[31] register
                                                          is equal to 1)                                                       */
      __IO uint32_t  UPC2       :  1;               /*!< When following conditions are true, release the fault output:
                                                         1. The FLTST register = 0x00. 2. The counter 0/ counter 1 has
                                                          matched comparator 2 While counting up. (Ignore item1 and FLTST
                                                          register is cleared to 0, when the FLTREL[31] register is equal
                                                          to 1)                                                                */
      __IO uint32_t  DOWMC3     :  1;               /*!< When following conditions are true, release the fault output:
                                                         1. The FLTST register = 0x00. 2. The counter 0/ counter 1 has
                                                          matched comparator 3 While counting down. (Ignore item1 and
                                                          FLTST register is cleared to 0, when the FLTREL[31] register
                                                          is equal to 1)                                                       */
      __IO uint32_t  UPC3       :  1;               /*!< When following conditions are true, release the fault output:
                                                         1. The FLTST register = 0x00. 2. The counter 0/ counter 1 has
                                                          matched comparator 3 While counting up. (Ignore item1 and FLTST
                                                          register is cleared to 0, when the FLTREL[31] register is equal
                                                          to 1)                                                                */
           uint32_t             :  4;
      __IO uint32_t  IMMED      :  1;               /*!< When following conditions are true, release the fault output:
                                                         1. The FLTST register = 0x00 (when the FLTREL[31] register is
                                                          equal to 0)                                                          */
           uint32_t             : 14;
      __IO uint32_t  MODE       :  1;               /*!< Fault release mode                                                    */
    } FLTREL_b;                                     /*!< BitSize                                                               */
  };
  
  union {
    __IO uint32_t  FLTST;                           /*!< PWM Fault Status Register                                             */
    
    struct {
      __IO uint32_t  FAULTA     :  1;               /*!< When set, the Fault A occurs event. Cleared by user.                  */
      __IO uint32_t  FAULTB     :  1;               /*!< When set, the Fault B occurs event. Cleared by user.                  */
      __IO uint32_t  FAULTC     :  1;               /*!< When set, the Fault B occurs event. Cleared by user.                  */
      __IO uint32_t  FAULTD     :  1;               /*!< When set, the Fault D occurs event. Cleared by user.                  */
    } FLTST_b;                                      /*!< BitSize                                                               */
  };
  
  union {
    __IO uint32_t  ICE;                             /*!< PWM ICE Mode Register                                                 */
    
    struct {
      __IO uint32_t  BKCU0      :  2;               /*!< When a breakpoint occurs.                                             */
           uint32_t             :  2;
      __IO uint32_t  BKCU1      :  4;               /*!< When a breakpoint occurs.                                             */
      __IO uint32_t  BK0A       :  1;               /*!< The PWM0A output signal (FLTOL[0]) on a breakpoint condition.         */
      __IO uint32_t  BK0B       :  1;               /*!< The PWM0B output signal (FLTOL[1]) on a breakpoint condition.         */
      __IO uint32_t  BK1A       :  1;               /*!< The PWM1A output signal (FLTOL[2]) on a breakpoint condition.         */
      __IO uint32_t  BK1B       :  1;               /*!< The PWM1B output signal (FLTOL[3]) on a breakpoint condition.         */
      __IO uint32_t  BK2A       :  1;               /*!< The PWM2A output signal (FLTOL[4]) on a breakpoint condition.         */
      __IO uint32_t  BK2B       :  1;               /*!< The PWM2B output signal (FLTOL[5]) on a breakpoint condition.         */
    } ICE_b;                                        /*!< BitSize                                                               */
  };
  
  union {
    __IO uint32_t  CUINIT0;                         /*!< PWM Counter Initial Register                                          */
    
    struct {
      __IO uint32_t  CUINIT     : 24;               /*!< The initial value of the counter x                                    */
    } CUINIT0_b;                                    /*!< BitSize                                                               */
  };
  
  union {
    __IO uint32_t  CUINIT1;                         /*!< PWM Counter Initial Register                                          */
    
    struct {
      __IO uint32_t  CUINIT     : 24;               /*!< The initial value of the counter x                                    */
    } CUINIT1_b;                                    /*!< BitSize                                                               */
  };
  
  union {
    __IO uint32_t  CTC;                             /*!< PWM Comparator Trigger Control                                        */
    
    struct {
      __IO uint32_t  EN0        :  1;               /*!< PWM to capture the analog comparator 0                                */
      __IO uint32_t  SS0        :  1;               /*!< Select the source of the comparator C0                                */
           uint32_t             :  6;
      __IO uint32_t  EN1        :  1;               /*!< PWM to capture the analog comparator 1                                */
      __IO uint32_t  SS1        :  1;               /*!< Select the source of the comparator C1                                */
           uint32_t             :  6;
      __IO uint32_t  EN2        :  1;               /*!< PWM to capture the analog comparator 2                                */
      __IO uint32_t  SS2        :  1;               /*!< Select the source of the comparator C2                                */
           uint32_t             :  6;
      __IO uint32_t  EN3        :  1;               /*!< PWM to capture the analog comparator 3                                */
      __IO uint32_t  SS3        :  1;               /*!< Select the source of the comparator C3                                */
    } CTC_b;                                        /*!< BitSize                                                               */
  };
  
  union {
    __IO uint32_t  ACMPC0;                          /*!< PWM Compare Cx Register                                               */
    
    struct {
      __IO uint32_t  ACMPC      : 24;               /*!< PWM Compare Cx Register                                               */
    } ACMPC0_b;                                     /*!< BitSize                                                               */
  };
  
  union {
    __IO uint32_t  ACMPC1;                          /*!< PWM Compare Cx Register                                               */
    
    struct {
      __IO uint32_t  ACMPC      : 24;               /*!< PWM Compare Cx Register                                               */
    } ACMPC1_b;                                     /*!< BitSize                                                               */
  };
  
  union {
    __IO uint32_t  ACMPC2;                          /*!< PWM Compare Cx Register                                               */
    
    struct {
      __IO uint32_t  ACMPC      : 24;               /*!< PWM Compare Cx Register                                               */
    } ACMPC2_b;                                     /*!< BitSize                                                               */
  };
  
  union {
    __IO uint32_t  ACMPC3;                          /*!< PWM Compare Cx Register                                               */
    
    struct {
      __IO uint32_t  ACMPC      : 24;               /*!< PWM Compare Cx Register                                               */
    } ACMPC3_b;                                     /*!< BitSize                                                               */
  };
  
  union {
    __IO uint32_t  OUTFIX;                          /*!< PWM Output FIX Register                                               */
    
    struct {
      __IO uint32_t  PWM0A      :  1;               /*!< When the OUTEN[0] is equal to 0                                       */
      __IO uint32_t  PWM0B      :  1;               /*!< When the OUTEN[1] is equal to 0                                       */
      __IO uint32_t  PWM1A      :  1;               /*!< When the OUTEN[2] is equal to 0                                       */
      __IO uint32_t  PWM1B      :  1;               /*!< When the OUTEN[3] is equal to 0                                       */
      __IO uint32_t  PWM2A      :  1;               /*!< When the OUTEN[4] is equal to 0                                       */
      __IO uint32_t  PWM2B      :  1;               /*!< When the OUTEN[5] is equal to 0                                       */
    } OUTFIX_b;                                     /*!< BitSize                                                               */
  };
  
  union {
    __IO uint32_t  OUTIVT;                          /*!< PWM Output Invert Register                                            */
    
    struct {
      __IO uint32_t  PWM0A      :  1;               /*!< PWM0A output pin invert bit                                           */
      __IO uint32_t  PWM0B      :  1;               /*!< PWM0B output pin invert bit                                           */
      __IO uint32_t  PWM1A      :  1;               /*!< PWM1A output pin invert bit                                           */
      __IO uint32_t  PWM1B      :  1;               /*!< PWM1B output pin invert bit                                           */
      __IO uint32_t  PWM2A      :  1;               /*!< PWM2A output pin invert bit                                           */
      __IO uint32_t  PWM2B      :  1;               /*!< PWM2B output pin invert bit                                           */
    } OUTIVT_b;                                     /*!< BitSize                                                               */
  };
} OB_PWM_Type;


/* ================================================================================ */
/* ================                       QEI                      ================ */
/* ================================================================================ */


/**
  * @brief Quadrature Encoder Interface (QEI)
  */

typedef struct {                                    /*!< QEI Structure                                                         */
  
  union {
    __IO uint32_t  CON;                             /*!< Control register                                                      */
    
    struct {
      __IO uint32_t  RESN       :  1;               /*!< QEI Enable                                                            */
      __IO uint32_t  RESP       :  1;               /*!< Reset position counter. When set = 1, resets the position counter
                                                         to all zeros. Autoclears when the position counter is cleared.        */
      __IO uint32_t  RESPI      :  1;               /*!< Reset position counter on index. When set = 1, resets the position
                                                         counter to all zeros when an index pulse occurs.                      */
      __IO uint32_t  RESV       :  1;               /*!< Reset velocity. When set = 1, resets the velocity counter and
                                                         velocity timer to all zeros. Autoclears when the velocity counter
                                                          is cleared.                                                          */
      __IO uint32_t  RESI       :  1;               /*!< Reset index counter. When set = 1, resets the index counter
                                                         to all zeros. Autoclears when the index counter is cleared.           */
    } CON_b;                                        /*!< BitSize                                                               */
  };
  
  union {
    __IO uint32_t  CONF;                            /*!< Configuration register                                                */
    
    struct {
      __IO uint32_t  DIRINV     :  1;               /*!< Direction invert. When = 1, complements the DIR bit.                  */
      __IO uint32_t  SIGMODE    :  1;               /*!< Signal Mode. When = 0, PhA and PhB function as quadrature encoder
                                                         inputs. When = 1, PhA functions as the direction signal and
                                                          PhB functions as the clock signal.                                   */
      __IO uint32_t  CAPMODE    :  1;               /*!< Capture Mode. When = 0, only PhA edges are counted (2X). When
                                                         = 1, BOTH PhA and PhB edges are counted (4X), increasing resolution
                                                          but decreasing range.                                                */
      __IO uint32_t  INVINX     :  1;               /*!< Invert Index. When set, inverts the sense of the index input.         */
    } CONF_b;                                       /*!< BitSize                                                               */
  };
  
  union {
    __I  uint32_t  ENSTA;                           /*!< Encoder status register                                               */
    
    struct {
      __I  uint32_t  DIR        :  1;               /*!< Direction bit. In combination with DIRINV bit indicates forward
                                                         or reverse direction.                                                 */
    } ENSTA_b;                                      /*!< BitSize                                                               */
  };
  
  union {
    __I  uint32_t  POS;                             /*!< Position register                                                     */
    
    struct {
      __I  uint32_t  POS_CNT    : 32;               /*!< Current position value.                                               */
    } POS_b;                                        /*!< BitSize                                                               */
  };
  
  union {
    __IO uint32_t  MAXPOS;                          /*!< Maximum position register                                             */
    
    struct {
      __IO uint32_t  MAXPOS     : 32;               /*!< Current maximum position value.                                       */
    } MAXPOS_b;                                     /*!< BitSize                                                               */
  };
  
  union {
    __IO uint32_t  CMPOS0;                          /*!< Position compare register 0                                           */
    
    struct {
      __IO uint32_t  CMPOS0     : 32;               /*!< Position compare value 0.                                             */
    } CMPOS0_b;                                     /*!< BitSize                                                               */
  };
  
  union {
    __IO uint32_t  CMPOS1;                          /*!< Position compare register 1                                           */
    
    struct {
      __IO uint32_t  CMPOS1     : 32;               /*!< Position compare value 1                                              */
    } CMPOS1_b;                                     /*!< BitSize                                                               */
  };
  
  union {
    __IO uint32_t  CMPOS2;                          /*!< Position compare register 2                                           */
    
    struct {
      __IO uint32_t  CMPOS2     : 32;               /*!< Position compare value 2                                              */
    } CMPOS2_b;                                     /*!< BitSize                                                               */
  };
  
  union {
    __I  uint32_t  INXCNT;                          /*!< Index count register                                                  */
    
    struct {
      __I  uint32_t  INXCNT     : 32;               /*!< Current index counter value.                                          */
    } INXCNT_b;                                     /*!< BitSize                                                               */
  };
  
  union {
    __IO uint32_t  INXCMP;                          /*!< Index compare register                                                */
    
    struct {
      __IO uint32_t  INXCMP     : 32;               /*!< Index compare value.                                                  */
    } INXCMP_b;                                     /*!< BitSize                                                               */
  };
  
  union {
    __IO uint32_t  VEMAXTIMER;                      /*!< Velocity Maximum timer register                                       */
    
    struct {
      __IO uint32_t  VEMAXTIMER : 32;               /*!< Velocity maximum timer value.                                         */
    } VEMAXTIMER_b;                                 /*!< BitSize                                                               */
  };
  
  union {
    __I  uint32_t  VETIME;                          /*!< Velocity timer register                                               */
    
    struct {
      __I  uint32_t  TIMECNT    : 32;               /*!< Current velocity timer value.                                         */
    } VETIME_b;                                     /*!< BitSize                                                               */
  };
  
  union {
    __I  uint32_t  VECU;                            /*!< Current velocity pulse count.                                         */
    
    struct {
      __I  uint32_t  VECU       : 32;               /*!< Current velocity pulse count.                                         */
    } VECU_b;                                       /*!< BitSize                                                               */
  };
  
  union {
    __I  uint32_t  VECAP;                           /*!< Velocity capture register                                             */
    
    struct {
      __I  uint32_t  VECAP      : 32;               /*!< Captured velocity pulse count.                                        */
    } VECAP_b;                                      /*!< BitSize                                                               */
  };
  
  union {
    __IO uint32_t  VECOMP;                          /*!< Velocity compare register                                             */
    
    struct {
      __IO uint32_t  VECOMP     : 32;               /*!< Velocity compare value.                                               */
    } VECOMP_b;                                     /*!< BitSize                                                               */
  };
  
  union {
    __IO uint32_t  FILTERA;                         /*!< Digital filter A register                                             */
    
    struct {
      __IO uint32_t  FILTERA    : 16;               /*!< Digital filter sampling delay.                                        */
    } FILTERA_b;                                    /*!< BitSize                                                               */
  };
  
  union {
    __IO uint32_t  FILTERB;                         /*!< Digital filter B register                                             */
    
    struct {
      __IO uint32_t  FILTERB    : 16;               /*!< Digital filter sampling delay.                                        */
    } FILTERB_b;                                    /*!< BitSize                                                               */
  };
  
  union {
    __IO uint32_t  FILTERI;                         /*!< Digital filter I register                                             */
    
    struct {
      __IO uint32_t  FILTERI    : 16;               /*!< Digital filter sampling delay.                                        */
    } FILTERI_b;                                    /*!< BitSize                                                               */
  };
  
  union {
    __I  uint32_t  IESTA;                           /*!< Interrupt status register                                             */
    
    struct {
      __I  uint32_t  INX        :  1;               /*!< Indicates that an index pulse was detected.                           */
      __I  uint32_t  TIM        :  1;               /*!< Indicates that a velocity timer overflow occurred.                    */
      __I  uint32_t  VELC       :  1;               /*!< Indicates that captured velocity is less than compare velocity.       */
      __I  uint32_t  DIR        :  1;               /*!< Indicates that a change of direction was detected.                    */
      __I  uint32_t  ERR        :  1;               /*!< Indicates that an encoder phase error was detected.                   */
      __I  uint32_t  ENCLK      :  1;               /*!< Indicates that and encoder clock pulse was detected.                  */
      __I  uint32_t  POS0       :  1;               /*!< Indicates that the position 0 compare value is equal to the
                                                         current position.                                                     */
      __I  uint32_t  POS1       :  1;               /*!< Indicates that the position 1 compare value is equal to the
                                                         current position.                                                     */
      __I  uint32_t  POS2       :  1;               /*!< Indicates that the position 2 compare value is equal to the
                                                         current position.                                                     */
      __I  uint32_t  REV        :  1;               /*!< Indicates that the index compare value is equal to the current
                                                         index count.                                                          */
      __I  uint32_t  POS0REV    :  1;               /*!< Combined position 0 and revolution count interrupt. Set when
                                                         both the POS0_Int bit is set and the REV_Int is set.                  */
      __I  uint32_t  POS1REV    :  1;               /*!< Combined position 1 and revolution count interrupt. Set when
                                                         both the POS1_Int bit is set and the REV_Int is set.                  */
      __I  uint32_t  POS2REV    :  1;               /*!< Combined position 2 and revolution count interrupt. Set when
                                                         both the POS2_Int bit is set and the REV_Int is set.                  */
    } IESTA_b;                                      /*!< BitSize                                                               */
  };
  
  union {
    __O  uint32_t  IESTAS;                          /*!< Interrupt status set register                                         */
    
    struct {
      __O  uint32_t  INX        :  1;               /*!< Indicates that an index pulse was detected.                           */
      __O  uint32_t  TIM        :  1;               /*!< Indicates that a velocity timer overflow occurred.                    */
      __O  uint32_t  VELC       :  1;               /*!< Indicates that captured velocity is less than compare velocity.       */
      __O  uint32_t  DIR        :  1;               /*!< Indicates that a change of direction was detected.                    */
      __O  uint32_t  ERR        :  1;               /*!< Indicates that an encoder phase error was detected.                   */
      __O  uint32_t  ENCLK      :  1;               /*!< Indicates that and encoder clock pulse was detected.                  */
      __O  uint32_t  POS0       :  1;               /*!< Indicates that the position 0 compare value is equal to the
                                                         current position.                                                     */
      __O  uint32_t  POS1       :  1;               /*!< Indicates that the position 1 compare value is equal to the
                                                         current position.                                                     */
      __O  uint32_t  POS2       :  1;               /*!< Indicates that the position 2 compare value is equal to the
                                                         current position.                                                     */
      __O  uint32_t  REV        :  1;               /*!< Indicates that the index compare value is equal to the current
                                                         index count.                                                          */
      __O  uint32_t  POS0REV    :  1;               /*!< Combined position 0 and revolution count interrupt. Set when
                                                         both the POS0_Int bit is set and the REV_Int is set.                  */
      __O  uint32_t  POS1REV    :  1;               /*!< Combined position 1 and revolution count interrupt. Set when
                                                         both the POS1_Int bit is set and the REV_Int is set.                  */
      __O  uint32_t  POS2REV    :  1;               /*!< Combined position 2 and revolution count interrupt. Set when
                                                         both the POS2_Int bit is set and the REV_Int is set.                  */
    } IESTAS_b;                                     /*!< BitSize                                                               */
  };
  
  union {
    __O  uint32_t  IESTAC;                          /*!< Interrupt status clear register                                       */
    
    struct {
      __O  uint32_t  INX        :  1;               /*!< Indicates that an index pulse was detected.                           */
      __O  uint32_t  TIM        :  1;               /*!< Indicates that a velocity timer overflow occurred.                    */
      __O  uint32_t  VELC       :  1;               /*!< Indicates that captured velocity is less than compare velocity.       */
      __O  uint32_t  DIR        :  1;               /*!< Indicates that a change of direction was detected.                    */
      __O  uint32_t  ERR        :  1;               /*!< Indicates that an encoder phase error was detected.                   */
      __O  uint32_t  ENCLK      :  1;               /*!< Indicates that and encoder clock pulse was detected.                  */
      __O  uint32_t  POS0       :  1;               /*!< Indicates that the position 0 compare value is equal to the
                                                         current position.                                                     */
      __O  uint32_t  POS1       :  1;               /*!< Indicates that the position 1 compare value is equal to the
                                                         current position.                                                     */
      __O  uint32_t  POS2       :  1;               /*!< Indicates that the position 2 compare value is equal to the
                                                         current position.                                                     */
      __O  uint32_t  REV        :  1;               /*!< Indicates that the index compare value is equal to the current
                                                         index count.                                                          */
      __O  uint32_t  POS0REV    :  1;               /*!< Combined position 0 and revolution count interrupt. Set when
                                                         both the POS0_Int bit is set and the REV_Int is set.                  */
      __O  uint32_t  POS1REV    :  1;               /*!< Combined position 1 and revolution count interrupt. Set when
                                                         both the POS1_Int bit is set and the REV_Int is set.                  */
      __O  uint32_t  POS2REV    :  1;               /*!< Combined position 2 and revolution count interrupt. Set when
                                                         both the POS2_Int bit is set and the REV_Int is set.                  */
    } IESTAC_b;                                     /*!< BitSize                                                               */
  };
  
  union {
    __I  uint32_t  IEEN;                            /*!< Interrupt enable register                                             */
    
    struct {
      __I  uint32_t  INX        :  1;               /*!< Indicates that an index pulse was detected.                           */
      __I  uint32_t  TIM        :  1;               /*!< Indicates that a velocity timer overflow occurred.                    */
      __I  uint32_t  VELC       :  1;               /*!< Indicates that captured velocity is less than compare velocity.       */
      __I  uint32_t  DIR        :  1;               /*!< Indicates that a change of direction was detected.                    */
      __I  uint32_t  ERR        :  1;               /*!< Indicates that an encoder phase error was detected.                   */
      __I  uint32_t  ENCLK      :  1;               /*!< Indicates that and encoder clock pulse was detected.                  */
      __I  uint32_t  POS0       :  1;               /*!< Indicates that the position 0 compare value is equal to the
                                                         current position.                                                     */
      __I  uint32_t  POS1       :  1;               /*!< Indicates that the position 1 compare value is equal to the
                                                         current position.                                                     */
      __I  uint32_t  POS2       :  1;               /*!< Indicates that the position 2 compare value is equal to the
                                                         current position.                                                     */
      __I  uint32_t  REV        :  1;               /*!< Indicates that the index compare value is equal to the current
                                                         index count.                                                          */
      __I  uint32_t  POS0REV    :  1;               /*!< Combined position 0 and revolution count interrupt.                   */
      __I  uint32_t  POS1REV    :  1;               /*!< Combined position 1 and revolution count interrupt.                   */
      __I  uint32_t  POS2REV    :  1;               /*!< Combined position 2 and revolution count interrupt.                   */
    } IEEN_b;                                       /*!< BitSize                                                               */
  };
  __I  uint32_t  RESERVED0;
  
  union {
    __O  uint32_t  IEENC;                           /*!< Interrupt enable clear register                                       */
    
    struct {
      __O  uint32_t  INX        :  1;               /*!< Indicates that an index pulse was detected.                           */
      __O  uint32_t  TIM        :  1;               /*!< Indicates that a velocity timer overflow occurred.                    */
      __O  uint32_t  VELC       :  1;               /*!< Indicates that captured velocity is less than compare velocity.       */
      __O  uint32_t  DIR        :  1;               /*!< Indicates that a change of direction was detected.                    */
      __O  uint32_t  ERR        :  1;               /*!< Indicates that an encoder phase error was detected.                   */
      __O  uint32_t  ENCLK      :  1;               /*!< Indicates that and encoder clock pulse was detected.                  */
      __O  uint32_t  POS0       :  1;               /*!< Indicates that the position 0 compare value is equal to the
                                                         current position.                                                     */
      __O  uint32_t  POS1       :  1;               /*!< Indicates that the position 1 compare value is equal to the
                                                         current position.                                                     */
      __O  uint32_t  POS2       :  1;               /*!< Indicates that the position 2 compare value is equal to the
                                                         current position.                                                     */
      __O  uint32_t  REV        :  1;               /*!< Indicates that the index compare value is equal to the current
                                                         index count.                                                          */
      __O  uint32_t  POS0REV    :  1;               /*!< Combined position 0 and revolution count interrupt.                   */
      __O  uint32_t  POS1REV    :  1;               /*!< Combined position 1 and revolution count interrupt.                   */
      __O  uint32_t  POS2REV    :  1;               /*!< Combined position 2 and revolution count interrupt.                   */
    } IEENC_b;                                      /*!< BitSize                                                               */
  };
  
  union {
    __IO uint32_t  OUC;                             /*!< This can be an increment when the position counter overflows
                                                         the MAXPOS value or a decrement when the position counter underflows
                                                          zero, depending on the direction of rotation.                        */
    
    struct {
      __IO uint32_t  OUC        : 31;               /*!< Over/Underflow Counter Register.                                      */
      __IO uint32_t  SIGN       :  1;               /*!< Signed bit                                                            */
    } OUC_b;                                        /*!< BitSize                                                               */
  };
} OB_QEI_Type;


/* ================================================================================ */
/* ================                       FMC                      ================ */
/* ================================================================================ */


/**
  * @brief Flash Memory Controller (FMC)
  */

typedef struct {                                    /*!< FMC Structure                                                         */
  
  union {
    __IO uint32_t  ISPCC;                           /*!< ISP Control / Command Register                                        */
    
    struct {
      __IO uint32_t  ISPEN      :  1;               /*!< ISP Enable (Write-Protection Bit)(R/W). ISP function enable
                                                         bit. Set this bit to enable ISP function. (note: Enable IROC
                                                          clock)                                                               */
           uint32_t             :  3;
      __IO uint32_t  ISPCMD     :  4;               /*!< ISP Command (R/W)                                                     */
           uint32_t             : 16;
      __IO uint32_t  ISPCONP    :  8;               /*!< Write protection of ISPCON (W). When write ISPCON[31:24] is
                                                         not equal 0xE8, the rest bits of ISPCON cannot be set.                */
    } ISPCC_b;                                      /*!< BitSize                                                               */
  };
  
  union {
    __IO uint32_t  ISPADR;                          /*!< ISP Address Register                                                  */
    
    struct {
      __IO uint32_t  ISPADR     : 32;               /*!< ISP Address. Maximum 32Kx32 (128 KB) of embedded Flash which
                                                         supports word program only. ISPADR[1:0] must be kept 00b for
                                                          ISP operation.                                                       */
    } ISPADR_b;                                     /*!< BitSize                                                               */
  };
  
  union {
    __IO uint32_t  ISPDAT;                          /*!< ISP Data Register                                                     */
    
    struct {
      __IO uint32_t  ISPDAT     : 32;               /*!< ISP Data. Write data to this register before ISP program operation.
                                                         Read data from this register after ISP read operation.                */
    } ISPDAT_b;                                     /*!< BitSize                                                               */
  };
  
  union {
    __IO uint32_t  ISPTS;                           /*!< ISP Trigger / Status Register                                         */
    
    struct {
      __IO uint32_t  ISPGO      :  1;               /*!< ISP Start Trigger (Write-protection Bit) (R/W). Write 1 to start
                                                         ISP operation and this bit will be cleared to 0 by hardware
                                                          automatically when ISP operation is finished.                        */
           uint32_t             :  3;
      __IO uint32_t  ISPFF      :  1;               /*!< ISP Fail Flag (Write-protection Bit) (R/W)                            */
           uint32_t             : 19;
      __IO uint32_t  ISPGOPRT   :  8;               /*!< Write protection of ISPGO (W). When write ISPTRG[31:24] is not
                                                         equal 0xE8, ISPGO cannot be set.                                      */
    } ISPTS_b;                                      /*!< BitSize                                                               */
  };
  __I  uint32_t  RESERVED0[12];
  
  union {
    __IO uint32_t  ICPENTRY;                        /*!< ICP entry.                                                            */
    
    struct {
      __IO uint32_t  ICPENTRY   : 32;               /*!< When write ICPENTRY [31:0] is not equal 0x1234567, ICPENTRY
                                                         cannot be set.                                                        */
    } ICPENTRY_b;                                   /*!< BitSize                                                               */
  };
  __I  uint32_t  RESERVED1[15];
  
  union {
    __I  uint32_t  MMCRPR;                          /*!< Main Memory Core Read Protect Register                                */
    
    struct {
      __I  uint32_t  CRPA       :  1;               /*!< Core reads protect for all 128 Kbyte. (register value)                */
    } MMCRPR_b;                                     /*!< BitSize                                                               */
  };
  
  union {
    __IO uint32_t  BSCSR;                           /*!< Boot Select Control / Status Register.                                */
    
    struct {
      __IO uint32_t  BS         :  2;               /*!< Boot Select (R/W)00/01 Boot from user code at flash.10 Boot
                                                         from ISP code at flash.11 Boot from ISP code at ram.                  */
           uint32_t             :  1;
      __IO uint32_t  ISPC       :  1;               /*!< ISP Code. FW set bit 1, reload function will read ISP Entry
                                                         (0x0001_FE04 ) status. After reload function, HW clear the bit
                                                          ( Write Only) Flash memory control : DFF status (Read Only).         */
    } BSCSR_b;                                      /*!< BitSize                                                               */
  };
} OB_FMC_Type;


/* ================================================================================ */
/* ================                       SPI                      ================ */
/* ================================================================================ */


/**
  * @brief Serial Peripheral Interface (SPI)
  */

typedef struct {                                    /*!< SPI Structure                                                         */
  
  union {
    __IO uint32_t  CR0;                             /*!< Control Register 0. Selects the serial clock rate, bus type,
                                                         and data size.                                                        */
    
    struct {
      __IO uint32_t  DSS        :  4;               /*!< Data Size Select. This field controls the number of bits transferred
                                                         in each frame. Values 0000-0010 are not supported and should
                                                          not be used.                                                         */
      __IO uint32_t  FRF        :  2;               /*!< Frame Format.                                                         */
      __IO uint32_t  CPOL       :  1;               /*!< Clock Out Polarity. This bit is only used in SPI mode.                */
      __IO uint32_t  CPHA       :  1;               /*!< Clock Out Phase. This bit is only used in SPI mode.                   */
      __IO uint32_t  SCR        :  8;               /*!< Serial Clock Rate. The number of prescaler output clocks per
                                                         bit on the bus, minus one. Given that CPSDVSR is the prescale
                                                          divider, and the APB clock PCLK clocks the prescaler, the bit
                                                          frequency is PCLK / (CPSDVSR X [SCR+1]).                             */
    } CR0_b;                                        /*!< BitSize                                                               */
  };
  
  union {
    __IO uint32_t  CR1;                             /*!< Control Register 1. Selects master/slave and other modes.             */
    
    struct {
           uint32_t             :  1;
      __IO uint32_t  SSE        :  1;               /*!< SPI Enable.                                                           */
      __IO uint32_t  MS         :  1;               /*!< Master/Slave Mode.This bit can only be written when the SSE
                                                         bit is 0.                                                             */
           uint32_t             :  5;
      __IO uint32_t  SSN        :  1;               /*!< Slave Select Negative (low active)                                    */
      __IO uint32_t  SSNP       :  1;               /*!< Slave Select Negative Pulse.                                          */
      __IO uint32_t  SSNCNTL    :  2;               /*!< Slave Select Negative Count Low. SSNP (bit-9) set, the timing
                                                         between Slave Select and SPI data (MSB or LSB).                       */
      __IO uint32_t  SSNCNTH    :  2;               /*!< Slave Select Negative Count High. SSNP (bit-9) set. the timing
                                                         at Slave Select High.                                                 */
    } CR1_b;                                        /*!< BitSize                                                               */
  };
  
  union {
    __IO uint32_t  DR;                              /*!< Data Register. Writes fill the transmit FIFO, and reads empty
                                                         the receive FIFO.                                                     */
    
    struct {
      __IO uint32_t  DATA       : 16;               /*!< Write: software can write data to be sent in a future frame
                                                         to this register whenever the TNF bit in the Status register
                                                          is 1, indicating that the Tx FIFO is not full. If the Tx FIFO
                                                          was previously empty and the SPI controller is not busy on the
                                                          bus, transmission of the data will begin immediately. Otherwise
                                                          the data written to this register will be sent as soon as all
                                                          previous data has been sent (and received). If the data length
                                                          is less than 16 bit, software must right-justify the data written
                                                          to this re                                                           */
    } DR_b;                                         /*!< BitSize                                                               */
  };
  
  union {
    __I  uint32_t  SR;                              /*!< Status Register                                                       */
    
    struct {
      __I  uint32_t  TFE        :  1;               /*!< Transmit FIFO Empty. This bit is 1 is the Transmit FIFO is empty,
                                                         0 if not.                                                             */
      __I  uint32_t  TNF        :  1;               /*!< Transmit FIFO Not Full. This bit is 0 if the Tx FIFO is full,
                                                         1 if not.                                                             */
      __I  uint32_t  RNE        :  1;               /*!< Receive FIFO Not Empty. This bit is 0 if the Receive FIFO is
                                                         empty, 1 if not.                                                      */
      __I  uint32_t  RFF        :  1;               /*!< Receive FIFO Full. This bit is 1 if the Receive FIFO is full,
                                                         0 if not.                                                             */
      __I  uint32_t  BSY        :  1;               /*!< Busy. This bit is 0 if the SPI controller is idle, 1 if it is
                                                         currently sending/receiving a frame and/or the Tx FIFO is not
                                                          empty.                                                               */
    } SR_b;                                         /*!< BitSize                                                               */
  };
  
  union {
    __IO uint32_t  CPSR;                            /*!< Clock Prescale Register                                               */
    
    struct {
      __IO uint32_t  CPSDVSR    :  8;               /*!< This even value between 2 and 254, by which SPI_PCLK is divided
                                                         to yield the prescaler output clock. Bit 0 always reads as 0.         */
    } CPSR_b;                                       /*!< BitSize                                                               */
  };
  
  union {
    __IO uint32_t  IMSC;                            /*!< Interrupt Mask Set and Clear Register                                 */
    
    struct {
      __IO uint32_t  RORIM      :  1;               /*!< Software should set this bit to enable interrupt when a Receive
                                                         Overrun occurs, that is, when the Rx FIFO is full and another
                                                          frame is completely received. The ARM spec implies that the
                                                          preceding frame data is overwritten by the new frame data when
                                                          this occurs.                                                         */
      __IO uint32_t  RTIM       :  1;               /*!< Software should set this bit to enable interrupt when a Receive
                                                         Time-out condition occurs. A Receive Time-out occurs when the
                                                          Rx FIFO is not empty, and no has not been read for a time-out
                                                          period. The time-out period is the same for master and slave
                                                          modes and is determined by the SSP bit rate: 32 bits at PCLK
                                                          / (CPSDVSR X [SCR+1]).                                               */
      __IO uint32_t  RXIM       :  1;               /*!< Software should set this bit to enable interrupt when the Rx
                                                         FIFO is at least half full.                                           */
      __IO uint32_t  TXIM       :  1;               /*!< Software should set this bit to enable interrupt when the Tx
                                                         FIFO is at least half empty.                                          */
    } IMSC_b;                                       /*!< BitSize                                                               */
  };
  
  union {
    __I  uint32_t  RIS;                             /*!< Raw Interrupt Status Register                                         */
    
    struct {
      __I  uint32_t  RORRIS     :  1;               /*!< This bit is 1 if another frame was completely received while
                                                         the RxFIFO was full. The ARM spec implies that the preceding
                                                          frame data is overwritten by the new frame data when this occurs.    */
      __I  uint32_t  RTRIS      :  1;               /*!< This bit is 1 if the Rx FIFO is not empty, and has not been
                                                         read for a time-out period. The time-out period is the same
                                                          for master and slave modes and is determined by the SSP bit
                                                          rate: 32 bits at PCLK / (CPSDVSR X [SCR+1]).                         */
      __I  uint32_t  RXRIS      :  1;               /*!< This bit is 1 if the Rx FIFO is at least half full.                   */
      __I  uint32_t  TXRIS      :  1;               /*!< This bit is 1 if the Tx FIFO is at least half empty.                  */
    } RIS_b;                                        /*!< BitSize                                                               */
  };
  
  union {
    __I  uint32_t  MIS;                             /*!< Masked Interrupt Status Register                                      */
    
    struct {
      __I  uint32_t  RORMIS     :  1;               /*!< This bit is 1 if another frame was completely received while
                                                         the RxFIFO was full, and this interrupt is enabled.                   */
      __I  uint32_t  RTMIS      :  1;               /*!< This bit is 1 if the Rx FIFO is not empty, has not been read
                                                         for a time-out period, and this interrupt is enabled. The time-out
                                                          period is the same for master and slave modes and is determined
                                                          by the SSP bit rate: 32 bits at PCLK / (CPSDVSR X [SCR+1]).          */
      __I  uint32_t  RXMIS      :  1;               /*!< This bit is 1 if the Rx FIFO is at least half full, and this
                                                         interrupt is enabled.                                                 */
      __I  uint32_t  TXMIS      :  1;               /*!< This bit is 1 if the Tx FIFO is at least half empty, and this
                                                         interrupt is enabled.                                                 */
    } MIS_b;                                        /*!< BitSize                                                               */
  };
  
  union {
    __O  uint32_t  ICR;                             /*!< ICR Interrupt Clear Register                                          */
    
    struct {
      __O  uint32_t  RORIC      :  1;               /*!< Writing a 1 to this bit clears the frame was received when RxFIFO
                                                         was full interrupt.                                                   */
      __O  uint32_t  RTIC       :  1;               /*!< Writing a 1 to this bit clears the Rx FIFO was not empty and
                                                         has not been read for a timeout period interrupt. The timeout
                                                          period is the same for master and slave modes and is determined
                                                          by the SSP bit rate: 32 bits at PCLK / (CPSDVSR X [SCR+1]).          */
    } ICR_b;                                        /*!< BitSize                                                               */
  };
  
  union {
    __IO uint32_t  CONDMA;                          /*!< Control DMA Register                                                  */
    
    struct {
           uint32_t             : 28;
      __IO uint32_t  DMARREQ    :  1;               /*!< DMA Receiver Request. (Read only)                                     */
      __IO uint32_t  DMATREQ    :  1;               /*!< DMA Transmitter Request. (Read only)                                  */
           uint32_t             :  1;
      __IO uint32_t  DMAE       :  1;               /*!< SPI DMA Enable.                                                       */
    } CONDMA_b;                                     /*!< BitSize                                                               */
  };
} OB_SPI_Type;


/* ================================================================================ */
/* ================                      ACMP                      ================ */
/* ================================================================================ */


/**
  * @brief Analog Comparator (ACMP)
  */

typedef struct {                                    /*!< ACMP Structure                                                        */
  
  union {
    __IO uint32_t  CON;                             /*!< Comparator Control Register                                           */
    
    struct {
      __IO uint32_t  EN0        :  1;               /*!< Comparator 0 function enable bit.                                     */
      __IO uint32_t  PSEL0      :  4;               /*!< Select Comparator 0 positive input source.                            */
      __IO uint32_t  NSEL0      :  3;               /*!< Select Comparator 0 negative input source                             */
      __IO uint32_t  EN1        :  1;               /*!< Comparator 1 function enable bit.                                     */
      __IO uint32_t  PSEL1      :  4;               /*!< Select Comparator 1 positive input source.                            */
      __IO uint32_t  NSEL1      :  3;               /*!< Select Comparator 1 negative input source                             */
      __IO uint32_t  EN2        :  1;               /*!< Comparator 2 function enable bit.                                     */
      __IO uint32_t  PSEL2      :  4;               /*!< Select Comparator 2 positive input source.                            */
      __IO uint32_t  NSEL2      :  3;               /*!< Select Comparator 2 negative input source                             */
      __IO uint32_t  EN3        :  1;               /*!< Comparator 3 function enable bit.                                     */
      __IO uint32_t  PSEL3      :  4;               /*!< Select Comparator 3 positive input source.                            */
      __IO uint32_t  NSEL3      :  3;               /*!< Select Comparator 3 negative input source                             */
    } CON_b;                                        /*!< BitSize                                                               */
  };
  
  union {
    __IO uint32_t  OUT;                             /*!< Comparator Output Register                                            */
    
    struct {
      __IO uint32_t  LATCHSEL0  :  1;               /*!< Comparator 0 data latch select                                        */
      __IO uint32_t  INV0       :  1;               /*!< Comparator 0 output pin invert bit.                                   */
           uint32_t             :  6;
      __IO uint32_t  LATCHSEL1  :  1;               /*!< Comparator 1 data latch select                                        */
      __IO uint32_t  INV1       :  1;               /*!< Comparator 1 output pin invert bit.                                   */
           uint32_t             :  6;
      __IO uint32_t  LATCHSEL2  :  1;               /*!< Comparator 2 data latch select                                        */
      __IO uint32_t  INV2       :  1;               /*!< Comparator 2 output pin invert bit.                                   */
           uint32_t             :  6;
      __IO uint32_t  LATCHSEL3  :  1;               /*!< Comparator 3 data latch select                                        */
      __IO uint32_t  INV3       :  1;               /*!< Comparator 3 output pin invert bit.                                   */
    } OUT_b;                                        /*!< BitSize                                                               */
  };
  
  union {
    __IO uint32_t  HYS;                             /*!< Comparator Hysteresis Register                                        */
    
    struct {
      __IO uint32_t  HYS0       :  3;               /*!< Comparator 0 with internal hysteresis.                                */
           uint32_t             :  5;
      __IO uint32_t  HYS1       :  3;               /*!< Comparator 1 with internal hysteresis.                                */
           uint32_t             :  5;
      __IO uint32_t  HYS2       :  3;               /*!< Comparator 2 with internal hysteresis.                                */
           uint32_t             :  5;
      __IO uint32_t  HYS3       :  3;               /*!< Comparator 3 with internal hysteresis.                                */
    } HYS_b;                                        /*!< BitSize                                                               */
  };
  
  union {
    __I  uint32_t  OUTSTA;                          /*!< Comparator Output Status Register                                     */
    
    struct {
      __I  uint32_t  OUT        :  4;               /*!< LATCHSEL output signal                                                */
      __I  uint32_t  SRC        :  4;               /*!< Comparator output value (Raw data)                                    */
      __I  uint32_t  LATCH      :  4;               /*!< PWM latch comparator data                                             */
    } OUTSTA_b;                                     /*!< BitSize                                                               */
  };
  
  union {
    __IO uint32_t  TADC0;                           /*!< Comparator Trigger ADC 0 Register                                     */
    
    struct {
      __IO uint32_t  FEN0       :  1;               /*!< The falling edge of the comparator 0 output to trigger ADC.           */
      __IO uint32_t  REN0       :  1;               /*!< The rising edge of the comparator 0 output to trigger ADC.            */
           uint32_t             :  6;
      __IO uint32_t  FEN1       :  1;               /*!< The falling edge of the comparator 1 output to trigger ADC.           */
      __IO uint32_t  REN1       :  1;               /*!< The rising edge of the comparator 1 output to trigger ADC.            */
           uint32_t             :  6;
      __IO uint32_t  FEN2       :  1;               /*!< The falling edge of the comparator 2 output to trigger ADC.           */
      __IO uint32_t  REN2       :  1;               /*!< The rising edge of the comparator 2 output to trigger ADC.            */
           uint32_t             :  6;
      __IO uint32_t  FEN3       :  1;               /*!< The falling edge of the comparator 3 output to trigger ADC.           */
      __IO uint32_t  REN3       :  1;               /*!< The rising edge of the comparator 3 output to trigger ADC.            */
    } TADC0_b;                                      /*!< BitSize                                                               */
  };
  __I  uint32_t  RESERVED0;
  
  union {
    __O  uint32_t  IEENS;                           /*!< Comparator Interrupt Enable Set Register                              */
    
    struct {
      __O  uint32_t  FIE0       :  1;               /*!< Write 1 to set interrupt enable (for comparator 0 falling edge)       */
      __O  uint32_t  RIE0       :  1;               /*!< Write 1 to set interrupt enable (for comparator 0 rising edge)        */
      __O  uint32_t  LIE0       :  1;               /*!< Write 1 to set interrupt enable (for PWM trigger comparator
                                                         0 latch data)                                                         */
           uint32_t             :  5;
      __O  uint32_t  FIE1       :  1;               /*!< Write 1 to set interrupt enable (for comparator 1 falling edge)       */
      __O  uint32_t  RIE1       :  1;               /*!< Write 1 to set interrupt enable (for comparator 1 rising edge)        */
      __O  uint32_t  LIE1       :  1;               /*!< Write 1 to set interrupt enable (for PWM trigger comparator
                                                         1 latch data)                                                         */
           uint32_t             :  5;
      __O  uint32_t  FIE2       :  1;               /*!< Write 1 to set interrupt enable (for comparator 2 falling edge)       */
      __O  uint32_t  RIE2       :  1;               /*!< Write 1 to set interrupt enable (for comparator 2 rising edge)        */
      __O  uint32_t  LIE2       :  1;               /*!< Write 1 to set interrupt enable (for PWM trigger comparator
                                                         2 latch data)                                                         */
           uint32_t             :  5;
      __O  uint32_t  FIE3       :  1;               /*!< Write 1 to set interrupt enable (for comparator 3 falling edge)       */
      __O  uint32_t  RIE3       :  1;               /*!< Write 1 to set interrupt enable (for comparator 3 rising edge)        */
      __O  uint32_t  LIE3       :  1;               /*!< Write 1 to set interrupt enable (for PWM trigger comparator
                                                         3 latch data)                                                         */
    } IEENS_b;                                      /*!< BitSize                                                               */
  };
  
  union {
    __O  uint32_t  IEENC;                           /*!< Comparator Interrupt Enable Clear Register                            */
    
    struct {
      __O  uint32_t  FIE0       :  1;               /*!< Write 1 to clear interrupt enable (for comparator 0 falling
                                                         edge)                                                                 */
      __O  uint32_t  RIE0       :  1;               /*!< Write 1 to clear interrupt enable (for comparator 0 rising edge)      */
      __O  uint32_t  LIE0       :  1;               /*!< Write 1 to clear interrupt enable (for PWM trigger comparator
                                                         0 latch data)                                                         */
           uint32_t             :  5;
      __O  uint32_t  FIE1       :  1;               /*!< Write 1 to clear interrupt enable (for comparator 1 falling
                                                         edge)                                                                 */
      __O  uint32_t  RIE1       :  1;               /*!< Write 1 to clear interrupt enable (for comparator 1 rising edge)      */
      __O  uint32_t  LIE1       :  1;               /*!< Write 1 to clear interrupt enable (for PWM trigger comparator
                                                         1 latch data)                                                         */
           uint32_t             :  5;
      __O  uint32_t  FIE2       :  1;               /*!< Write 1 to clear interrupt enable (for comparator 2 falling
                                                         edge)                                                                 */
      __O  uint32_t  RIE2       :  1;               /*!< Write 1 to clear interrupt enable (for comparator 2 rising edge)      */
      __O  uint32_t  LIE2       :  1;               /*!< Write 1 to clear interrupt enable (for PWM trigger comparator
                                                         2 latch data)                                                         */
           uint32_t             :  5;
      __O  uint32_t  FIE3       :  1;               /*!< Write 1 to clear interrupt enable (for comparator 3 falling
                                                         edge)                                                                 */
      __O  uint32_t  RIE3       :  1;               /*!< Write 1 to clear interrupt enable (for comparator 3 rising edge)      */
      __O  uint32_t  LIE3       :  1;               /*!< Write 1 to clear interrupt enable (for PWM trigger comparator
                                                         3 latch data)                                                         */
    } IEENC_b;                                      /*!< BitSize                                                               */
  };
  
  union {
    __I  uint32_t  IEST;                            /*!< Comparator Interrupt Status Register                                  */
    
    struct {
      __I  uint32_t  FIE0       :  1;               /*!< The falling edge of the comparator 0 output to generate an interrupt  */
      __I  uint32_t  RIE0       :  1;               /*!< The rising edge of the comparator 0 output to generate an interrupt   */
      __I  uint32_t  LIE0       :  1;               /*!< PWM trigger comparator 0 latch data output to generate an interrupt   */
           uint32_t             :  5;
      __I  uint32_t  FIE1       :  1;               /*!< The falling edge of the comparator 1 output to generate an interrupt  */
      __I  uint32_t  RIE1       :  1;               /*!< The rising edge of the comparator 1 output to generate an interrupt.  */
      __I  uint32_t  LIE1       :  1;               /*!< PWM trigger comparator 1 latch data output to generate an interrupt   */
           uint32_t             :  5;
      __I  uint32_t  FIE2       :  1;               /*!< The falling edge of the comparator 2 output to generate an interrupt. */
      __I  uint32_t  RIE2       :  1;               /*!< The rising edge of the comparator 2 output to generate an interrupt.  */
      __I  uint32_t  LIE2       :  1;               /*!< PWM trigger comparator 2 latch data output to generate an interrupt   */
           uint32_t             :  5;
      __I  uint32_t  FIE3       :  1;               /*!< The falling edge of the comparator 3 output to generate an interrupt. */
      __I  uint32_t  RIE3       :  1;               /*!< The rising edge of the comparator 3 output to generate an interrupt.  */
      __I  uint32_t  LIE3       :  1;               /*!< PWM trigger comparator 3 latch data output to generate an interrupt   */
    } IEST_b;                                       /*!< BitSize                                                               */
  };
  
  union {
    __O  uint32_t  IESTC;                           /*!< Comparator Interrupt Status Clear Register                            */
    
    struct {
      __O  uint32_t  FIE0       :  1;               /*!< Write 1 to clear the bit corresponding to IEST.                       */
      __O  uint32_t  RIE0       :  1;               /*!< Write 1 to clear the bit corresponding to IEST.                       */
      __O  uint32_t  LIE0       :  1;               /*!< Write 1 to clear the bit corresponding to IEST.                       */
           uint32_t             :  5;
      __O  uint32_t  FIE1       :  1;               /*!< Write 1 to clear the bit corresponding to IEST.                       */
      __O  uint32_t  LIE1       :  1;               /*!< Write 1 to clear the bit corresponding to IEST.                       */
           uint32_t             :  6;
      __O  uint32_t  FIE2       :  1;               /*!< Write 1 to clear the bit corresponding to IEST.                       */
      __O  uint32_t  RIE2       :  1;               /*!< Write 1 to clear the bit corresponding to IEST.                       */
      __O  uint32_t  LIE2       :  1;               /*!< Write 1 to clear the bit corresponding to IEST.                       */
           uint32_t             :  5;
      __O  uint32_t  FIE3       :  1;               /*!< Write 1 to clear the bit corresponding to IEST.                       */
      __O  uint32_t  RIE3       :  1;               /*!< Write 1 to clear the bit corresponding to IEST.                       */
      __O  uint32_t  LIE3       :  1;               /*!< Write 1 to clear the bit corresponding to IEST.                       */
    } IESTC_b;                                      /*!< BitSize                                                               */
  };
  __I  uint32_t  RESERVED1[2];
  
  union {
    __IO uint32_t  DBCNT0;                          /*!< De-Bounce Counter Register                                            */
    
    struct {
      __IO uint32_t  DBCNT      : 16;               /*!< Comparator x de-bounce counter (note : value = 0, no de-bounce
                                                         function)                                                             */
    } DBCNT0_b;                                     /*!< BitSize                                                               */
  };
  
  union {
    __IO uint32_t  DBCNT1;                          /*!< De-Bounce Counter Register                                            */
    
    struct {
      __IO uint32_t  DBCNT      : 16;               /*!< Comparator x de-bounce counter (note : value = 0, no de-bounce
                                                         function)                                                             */
    } DBCNT1_b;                                     /*!< BitSize                                                               */
  };
  
  union {
    __IO uint32_t  DBCNT2;                          /*!< De-Bounce Counter Register                                            */
    
    struct {
      __IO uint32_t  DBCNT      : 16;               /*!< Comparator x de-bounce counter (note : value = 0, no de-bounce
                                                         function)                                                             */
    } DBCNT2_b;                                     /*!< BitSize                                                               */
  };
  
  union {
    __IO uint32_t  DBCNT3;                          /*!< De-Bounce Counter Register                                            */
    
    struct {
      __IO uint32_t  DBCNT      : 16;               /*!< Comparator x de-bounce counter (note : value = 0, no de-bounce
                                                         function)                                                             */
    } DBCNT3_b;                                     /*!< BitSize                                                               */
  };
} OB_ACMP_Type;


/* ================================================================================ */
/* ================                       OPA                      ================ */
/* ================================================================================ */


/**
  * @brief OPA (OPA)
  */

typedef struct {                                    /*!< OPA Structure                                                         */
  
  union {
    __IO uint32_t  CON;                             /*!< OPA Control Register                                                  */
    
    struct {
      __IO uint32_t  EN0        :  1;               /*!< OPA 0 function enable bit                                             */
           uint32_t             :  7;
      __IO uint32_t  EN1        :  1;               /*!< OPA 1 function enable bit                                             */
           uint32_t             :  7;
      __IO uint32_t  EN2        :  1;               /*!< OPA 2 function enable bit                                             */
           uint32_t             :  7;
      __IO uint32_t  EN3        :  1;               /*!< OPA 3 function enable bit                                             */
    } CON_b;                                        /*!< BitSize                                                               */
  };
} OB_OPA_Type;


/* ================================================================================ */
/* ================                       RTC                      ================ */
/* ================================================================================ */


/**
  * @brief Real Time Controller (RTC)
  */

typedef struct {                                    /*!< RTC Structure                                                         */
  
  union {
    __O  uint32_t  WTKEY1;                          /*!< RTC Write Time Key Register.                                          */
    
    struct {
      __O  uint32_t  WTKEY      :  7;               /*!< Software must write 00h to the WTKEY1, 0Bh to the WTKEY2, register
                                                         to enable the time register's write attribute (08h ~ 20h).            */
    } WTKEY1_b;                                     /*!< BitSize                                                               */
  };
  
  union {
    __O  uint32_t  WTKEY2;                          /*!< RTC Write Time Key Register.                                          */
    
    struct {
      __O  uint32_t  WTKEY      :  7;               /*!< Software must write 00h to the WTKEY1, 0Bh to the WTKEY2, register
                                                         to enable the time register's write attribute (08h ~ 20h).            */
    } WTKEY2_b;                                     /*!< BitSize                                                               */
  };
  
  union {
    __IO uint32_t  SEC;                             /*!< RTC Seconds Register.                                                 */
    
    struct {
      __IO uint32_t  SECONDS    :  7;               /*!< Contains the BCD value of seconds. Lower nibble contains the
                                                         lower digit and operates from 0 to 9; upper nibble contains
                                                          the upper digit and operates from 0 to 5. The range for the
                                                          register is 0~59.                                                    */
    } SEC_b;                                        /*!< BitSize                                                               */
  };
  
  union {
    __IO uint32_t  MIN;                             /*!< RTC Minutes Register.                                                 */
    
    struct {
      __IO uint32_t  MINUTES    :  7;               /*!< Contains the BCD value of minutes. Low nibble contains the lower
                                                         digit and operates from 0 to 9; upper nibble contains the upper
                                                          digit and operates from 0 to 5. The range for the register is
                                                          0~59.                                                                */
    } MIN_b;                                        /*!< BitSize                                                               */
  };
  
  union {
    __IO uint32_t  HOUR;                            /*!< RTC Hour Register.                                                    */
    
    struct {
      __IO uint32_t  HOURS      :  6;               /*!< Contains the BCD value of hours in 24-hour format. Lower nibble
                                                         contains the lower digit and operates from 0 to 9; upper nibble
                                                          (two bits) contains the upper digit and operates from 0 to 2.
                                                          The range for the register is 0~23.                                  */
    } HOUR_b;                                       /*!< BitSize                                                               */
  };
  
  union {
    __IO uint32_t  DATE;                            /*!< RTC Dates Register.                                                   */
    
    struct {
      __IO uint32_t  DATES      :  6;               /*!< Contains the BCD value for the date of the month. Lower nibble
                                                         contains the lower digit and operates from 0 to 9; upper nibble
                                                          (two bits) contains the upper digit and operates from 0 to 3.
                                                          The range for the register is 1~31.                                  */
    } DATE_b;                                       /*!< BitSize                                                               */
  };
  
  union {
    __IO uint32_t  DAY;                             /*!< RTC Days Register.                                                    */
    
    struct {
      __IO uint32_t  DAYS       :  3;               /*!< Lower nibble contains a value that correlate to day of the week.
                                                         Day of the week is a ring counter that counts from 0 to 6 then
                                                          returns to 0. The user must assign meaning to the day value,
                                                          as the day is not integrated with the date.                          */
    } DAY_b;                                        /*!< BitSize                                                               */
  };
  
  union {
    __IO uint32_t  MON;                             /*!< RTC Months Register.                                                  */
    
    struct {
      __IO uint32_t  MONTHS     :  5;               /*!< Contains the BCD for the months. Lower nibble contains the lower
                                                         digit and operates from 0 to 9; upper nibble (one bit) contains
                                                          the upper digit and operates from 0 to 1. The range from the
                                                          register is 1~12.                                                    */
    } MON_b;                                        /*!< BitSize                                                               */
  };
  
  union {
    __IO uint32_t  YEAR;                            /*!< RTC Years Register.                                                   */
    
    struct {
      __IO uint32_t  YEARS      :  8;               /*!< Contains the lower two BCD digit of the years. Lower nibble
                                                         contains the value for years; upper nibble contains the value
                                                          for 10s of years. Each nibble operates from 0 to 9. The range
                                                          for the register is 0-99.                                            */
    } YEAR_b;                                       /*!< BitSize                                                               */
  };
  
  union {
    __IO uint32_t  LATCH;                           /*!< RTC Latch Register.                                                   */
    
    struct {
      __IO uint32_t  LATCH      :  8;               /*!< Value 0xC0. Software must read the LATCH register before reading
                                                         the time registers' data (08 ~ 20h).                                  */
    } LATCH_b;                                      /*!< BitSize                                                               */
  };
  
  union {
    __IO uint32_t  ADJ;                             /*!< RTC Adjust Register.                                                  */
    
    struct {
      __IO uint32_t  ADJUST     : 15;               /*!< Calibration value. (2's complement) TKPSEL=1, 30 second calibration:
                                                         ADJUST[12:0] valid; TKPSEL=0, 20 second calibration: ADJUST[11:0]
                                                          valid; other reserve (SIGN except)                                   */
           uint32_t             : 16;
      __IO uint32_t  SIGN       :  1;               /*!< Calibration sign. Determines if the calibration adjustment is
                                                         applied as an addition to or as a subtraction from the time-base.     */
    } ADJ_b;                                        /*!< BitSize                                                               */
  };
  __I  uint32_t  RESERVED0;
  
  union {
    __IO uint32_t  ALSEC;                           /*!< RTC Alarm Seconds Register.                                           */
    
    struct {
      __IO uint32_t  ALSEC      :  7;               /*!< Contain the alarm value for the seconds. Lower nibble contains
                                                         the lower digit and operates from 0 to 9; upper nibble contains
                                                          the upper digit and operates from 0 to 5. The range for the
                                                          register is 0~59.                                                    */
    } ALSEC_b;                                      /*!< BitSize                                                               */
  };
  
  union {
    __IO uint32_t  ALMIN;                           /*!< RTC Alarm Minutes Register.                                           */
    
    struct {
      __IO uint32_t  ALMIN      :  7;               /*!< Contains the alarm value for the minutes. Lower nibble contains
                                                         the lower digit and operates from 0 to 9; upper nibble contains
                                                          the upper digit and operates from 0 to 5. The range for the
                                                          register is 0~59.                                                    */
    } ALMIN_b;                                      /*!< BitSize                                                               */
  };
  
  union {
    __IO uint32_t  ALHOUR;                          /*!< RTC Alarm Hours Register.                                             */
    
    struct {
      __IO uint32_t  ALHOUR     :  6;               /*!< Contains the alarm value for the hours. Lower nibble contains
                                                         the lower digit and operates from 0 to 9; upper nibble (two
                                                          bits) contains the upper digit and operates from 0 to 2. The
                                                          range for the register is 0~23.                                      */
    } ALHOUR_b;                                     /*!< BitSize                                                               */
  };
  __I  uint32_t  RESERVED1;
  
  union {
    __IO uint32_t  IEENS;                           /*!< RTC Interrupt Enable Set Register.                                    */
    
    struct {
      __IO uint32_t  SIE        :  1;               /*!< When set, the seconds interrupt is enabled.                           */
      __IO uint32_t  MIE        :  1;               /*!< When set, the minutes interrupt is enabled.                           */
      __IO uint32_t  HIE        :  1;               /*!< When set, the hours interrupt is enabled.                             */
           uint32_t             :  5;
      __IO uint32_t  ALME       :  1;               /*!< When set, the alarm interrupt is enabled.                             */
    } IEENS_b;                                      /*!< BitSize                                                               */
  };
  
  union {
    __IO uint32_t  IEENC;                           /*!< RTC Interrupt Enable Clear Register.                                  */
    
    struct {
      __IO uint32_t  SIE        :  1;               /*!< When set, the seconds interrupt is disabled.                          */
      __IO uint32_t  MIE        :  1;               /*!< When set, the minutes interrupt is disabled.                          */
      __IO uint32_t  HIE        :  1;               /*!< When set, the hours interrupt is disabled.                            */
           uint32_t             :  5;
      __IO uint32_t  ALME       :  1;               /*!< When set, the alarm interrupt is disabled.                            */
    } IEENC_b;                                      /*!< BitSize                                                               */
  };
  
  union {
    __IO uint32_t  IEST;                            /*!< RTC Interrupt Status Register.                                        */
    
    struct {
      __IO uint32_t  SIF        :  1;               /*!< Seconds Interrupt Flag. If SIE (the seconds interrupt is enabled)
                                                         is written to logic 1. Ripple count from 32767 to 0, hardware
                                                          set SIF flag from 0 to1. When Set, The second value change from
                                                          59 to 0                                                              */
      __IO uint32_t  MIF        :  1;               /*!< Minutes Interrupt Flag. If MIE (the the minutes interrupt is
                                                         enabled) is written to logic 1. Second change from 59 to 0,
                                                          hardware set MIF flag from 0 to 1.                                   */
      __IO uint32_t  HIF        :  1;               /*!< Hours Interrupt Flag. If HIE (the hours interrupt is enabled)
                                                         is written to logic 1. Minute change from 59 to 0, hardware
                                                          set HIF flag from 0 to 1.                                            */
           uint32_t             :  5;
      __IO uint32_t  ALMIF      :  1;               /*!< Alarm Interrupt flag. If ALMIE (the alarm interrupt is enabled)
                                                         is written to logic 1, the ALMIF is the result of a match with
                                                          seconds, minutes and hours. When set, The Alarm Value has matched
                                                          (seconds, minutes and hours)                                         */
    } IEST_b;                                       /*!< BitSize                                                               */
  };
  
  union {
    __IO uint32_t  IESTC;                           /*!< RTC Interrupt Status Clear Register.                                  */
    
    struct {
      __IO uint32_t  SIFC       :  1;               /*!< When clear, the Seconds Interrupt Flag is cleared.                    */
      __IO uint32_t  MIFC       :  1;               /*!< When clear, the Minutes Interrupt Flag is cleared.                    */
      __IO uint32_t  HIFC       :  1;               /*!< When clear, the Hours Interrupt Flag is cleared.                      */
           uint32_t             :  5;
      __IO uint32_t  ALMIFC     :  1;               /*!< When clear, the Alarm Interrupt flag is cleared.                      */
    } IESTC_b;                                      /*!< BitSize                                                               */
  };
  __I  uint32_t  RESERVED2;
  
  union {
    __IO uint32_t  CON;                             /*!< RTC Control Register.                                                 */
    
    struct {
      __IO uint32_t  XAMP       :  1;               /*!< Crystal osc amplifier SFR option from RTC controller.                 */
           uint32_t             :  2;
      __IO uint32_t  TKPSEL     :  1;               /*!< Time Keeping select function.                                         */
           uint32_t             :  1;
      __IO uint32_t  OSFDET     :  1;               /*!< Crystal osc failure detector enable signal from RTC controller.       */
           uint32_t             :  1;
      __IO uint32_t  XPD        :  1;               /*!< Power down enable signal of crystal osc from RTC controller.          */
    } CON_b;                                        /*!< BitSize                                                               */
  };
  __I  uint32_t  RESERVED3[10];
  
  union {
    __I  uint32_t  CALCNT;                          /*!< Calibration Counter.                                                  */
    
    struct {
      __I  uint32_t  CALCNT     : 32;               /*!< Calibration counter                                                   */
    } CALCNT_b;                                     /*!< BitSize                                                               */
  };
  
  union {
    __IO uint32_t  CALCON;                          /*!< Calibration Control.                                                  */
    
    struct {
      __IO uint32_t  RCCE       :  1;               /*!< RTC calibration enable, auto clears by hardware.                      */
           uint32_t             :  3;
      __IO uint32_t  O1HZE      :  1;               /*!< RTC 1HZ Output enable (note P10 input/output).                        */
           uint32_t             :  3;
      __IO uint32_t  CALCNTSEL  :  3;               /*!< Calibration count select.                                             */
    } CALCON_b;                                     /*!< BitSize                                                               */
  };
} OB_RTC_Type;


/* ================================================================================ */
/* ================                      PDMA                      ================ */
/* ================================================================================ */


/**
  * @brief PDMA 0~3 (PDMA)
  */

typedef struct {                                    /*!< PDMA Structure                                                        */
  
  union {
    __IO uint32_t  PDMA_SRCADR;                     /*!< PDMA Destination Address Register CHX.                                */
    
    struct {
      __IO uint32_t  PDMA_SRCADR: 32;               /*!< Source addresses of the current DMA cycles. Note : The source
                                                         address must be word alignment.                                       */
    } PDMA_SRCADR_b;                                /*!< BitSize                                                               */
  };
  
  union {
    __IO uint32_t  PDMA_DESADR;                     /*!< PDMA Destination Address Register CHX.                                */
    
    struct {
      __IO uint32_t  PDMA_DESADR: 32;               /*!< Destination addresses of the current DMA cycles. Note : The
                                                         source address must be word alignment.                                */
    } PDMA_DESADR_b;                                /*!< BitSize                                                               */
  };
  
  union {
    __IO uint32_t  PDMA_CYC;                        /*!< PDMA Transfer Cycle Register CHX.                                     */
    
    struct {
      __IO uint32_t  PDMA_CYC   : 24;               /*!< A DMA cycle consists of one or four bus data transfer cycles,
                                                         depending on the command burst bit (BURMOD) in the PDMA_CSR
                                                          register.                                                            */
    } PDMA_CYC_b;                                   /*!< BitSize                                                               */
  };
  
  union {
    __IO uint32_t  PDMA_CSR;                        /*!< PDMA Control and Status Register CHX.                                 */
    
    struct {
      __IO uint32_t  ENB        :  1;               /*!< Enable DMA transfer. This bit can be cleared automatically when
                                                         all DMA cycles are finished. Please note that when CPU stops
                                                          the DMA transfers by setting this bit to '0' before all DMA
                                                          cycles are finished. Before CPU reconfigures another DMA transfer,
                                                          CPU needs to monitor this bit to ensure this bit is cleared
                                                          by the DMA state machine.                                            */
      __IO uint32_t  FININTSTS  :  1;               /*!< Finishing interrupt flag. This bit can be cleared manually by
                                                         writing a '1' to it.                                                  */
      __IO uint32_t  FININTENB  :  1;               /*!< Finishing interrupt control.                                          */
      __IO uint32_t  BURMOD     :  1;               /*!< Burst mode control                                                    */
      __IO uint32_t  ERRINTSTS  :  1;               /*!< Error response interrupt flag. This bit can be cleared manually
                                                         by writing a '1' to it.                                               */
      __IO uint32_t  ERRINTENB  :  1;               /*!< Interrupt when the AHB slave returns an error response during
                                                         a DMA transfer.                                                       */
      __IO uint32_t  SRCADRSEL  :  1;               /*!< Source address control.                                               */
      __IO uint32_t  DESADRSEL  :  1;               /*!< Destination address control.                                          */
      __IO uint32_t  SRCADRINC  :  3;               /*!< Source address incremental.                                           */
           uint32_t             :  1;
      __IO uint32_t  DESADRINC  :  3;               /*!< Destination address incremental.                                      */
           uint32_t             :  1;
      __IO uint32_t  DESREQSEL  :  4;               /*!< Request/Grant signal to select of the destination address in
                                                         the DMA hardware handshake mode.                                      */
      __IO uint32_t  DATAWIDTH  :  2;               /*!< Data width of the transfer. Please note that when the DMA source
                                                         or the destination is APB device, the data width must be set
                                                          to word. The APB device supports the word-width transfer only.       */
           uint32_t             :  2;
      __IO uint32_t  SRCREQSEL  :  4;               /*!< Request/Grant signal to select of the source address in the
                                                         DMA hardware handshake mode.                                          */
      __IO uint32_t  PSLVERR    :  1;               /*!< Error response interrupt flag for APB slave. This bit can be
                                                         cleared manually by writing a '1' to it.                              */
      __IO uint32_t  PSLVERREN  :  1;               /*!< Slave error when the APB slave returns an error response during
                                                         a DMA transfer.                                                       */
    } PDMA_CSR_b;                                   /*!< BitSize                                                               */
  };
} OB_PDMA_Type;


/* ================================================================================ */
/* ================                       CRC                      ================ */
/* ================================================================================ */


/**
  * @brief Cyclic Redundancy Check (CRC)
  */

typedef struct {                                    /*!< CRC Structure                                                         */
  
  union {
    __IO uint32_t  CTL;                             /*!< CRC Control Register.                                                 */
    
    struct {
      __IO uint32_t  EN         :  1;               /*!< CRC Enable. Note1: When operating in CRC DMA mode, EN will start
                                                         CRC engine to calculate.                                              */
      __IO uint32_t  RST        :  1;               /*!< CRC Engine Reset. Note1: When operating in CPU PIO mode, setting
                                                         this bit will reload the initial seed value. Note2: When operating
                                                          in CRC DMA mode, CRC will recalculate.                               */
           uint32_t             :  2;
      __IO uint32_t  PREBREV    :  1;               /*!< Bit Reversed for Input Previous CRC calculation result.               */
           uint32_t             : 15;
      __IO uint32_t  DMASEL     :  2;               /*!< DMA channel selection                                                 */
      __IO uint32_t  ENGINEFSB  :  1;               /*!< The first serial bit selection for CRC Engine.                        */
      __IO uint32_t  MODESEL    :  1;               /*!< CRC Mode selection                                                    */
      __IO uint32_t  WD         :  1;               /*!< Write Data Order Reversed. Note: If the write data is 0xAABBCCDD,
                                                         the bit order reverse for CRC write data in is 0x55DD33BB.            */
      __IO uint32_t  CSR        :  1;               /*!< Checksum Reversed. Note: If the checksum data is 0XDD7B0F2E,
                                                         the bit order reverse for CRC checksum is 0x74F0DEBB.                 */
      __IO uint32_t  WDC        :  1;               /*!< Write Data Complement.                                                */
      __IO uint32_t  CSC        :  1;               /*!< Checksum Complement.                                                  */
      __IO uint32_t  CRWDLEN    :  2;               /*!< CPU Write Data Length.                                                */
      __IO uint32_t  MODE       :  2;               /*!< CRC Polynomial Mode.                                                  */
    } CTL_b;                                        /*!< BitSize                                                               */
  };
  
  union {
    __IO uint32_t  OCR;                             /*!< CRC Offset Cycle Register.                                            */
    
    struct {
      __IO uint32_t  OCR        : 32;               /*!< CRC Offset Cycle Register.                                            */
    } OCR_b;                                        /*!< BitSize                                                               */
  };
  
  union {
    __IO uint32_t  TCR;                             /*!< CRC Transfer Cycle Register.                                          */
    
    struct {
      __IO uint32_t  TCR        : 32;               /*!< CRC Transfer Cycle Count Register.                                    */
    } TCR_b;                                        /*!< BitSize                                                               */
  };
  
  union {
    __IO uint32_t  DMAIER;                          /*!< CRC Interrupt Enable Register.                                        */
    
    struct {
      __IO uint32_t  EN         :  1;               /*!< Interrupt generator enabled during CRC DMA transfer done.             */
    } DMAIER_b;                                     /*!< BitSize                                                               */
  };
  
  union {
    __IO uint32_t  DMAISR;                          /*!< CRC Interrupt Status Register.                                        */
    
    struct {
      __IO uint32_t  BLKD       :  1;               /*!< This bit indicates that CRC DMA has finished all transfer. Note
                                                         : Software can write 1 to clear this bit to zero.                     */
    } DMAISR_b;                                     /*!< BitSize                                                               */
  };
  
  union {
    __IO uint32_t  WD;                              /*!< CRC Write Data.                                                       */
    
    struct {
      __IO uint32_t  WD         : 32;               /*!< When operating in the CPU PIO mode, software can write data
                                                         to this field to perform CRC operation. When operating in the
                                                          CRC DMA mode, this field will be used for DMA internal buffer.
                                                          Note1: When operating in the CRC DMA mode, so don't filled any
                                                          data in this field. Note2:The CTL [WDC] and CTL [WD] bit setting
                                                          will affected this field; For example, if CTL [WD] = 1, if the
                                                          write data in CTL [WD] register is 0xAABBCCDD.                       */
    } WD_b;                                         /*!< BitSize                                                               */
  };
  
  union {
    __IO uint32_t  SEED;                            /*!< CRC Seed Register.                                                    */
    
    struct {
      __IO uint32_t  SEED       : 32;               /*!< CRC Seed Register.                                                    */
    } SEED_b;                                       /*!< BitSize                                                               */
  };
  
  union {
    __IO uint32_t  CS;                              /*!< CRC Check Sum Register.                                               */
    
    struct {
      __IO uint32_t  CS         : 32;               /*!< CRC Checksum Register.                                                */
    } CS_b;                                         /*!< BitSize                                                               */
  };
} OB_CRC_Type;


/* ================================================================================ */
/* ================                      GPIO                      ================ */
/* ================================================================================ */


/**
  * @brief GPIO (GPIO)
  */

typedef struct {                                    /*!< GPIO Structure                                                        */
  
  union {
    __IO uint32_t  DATA;                            /*!< GPIO Port X Data Register.                                            */
    
    struct {
      __IO uint32_t  GPIOXDATA0 :  1;               /*!< Output data for pins GPIOn_0                                          */
      __IO uint32_t  GPIOXDATA1 :  1;               /*!< Output data for pins GPIOn_1                                          */
      __IO uint32_t  GPIOXDATA2 :  1;               /*!< Output data for pins GPIOn_2                                          */
      __IO uint32_t  GPIOXDATA3 :  1;               /*!< Output data for pins GPIOn_3                                          */
      __IO uint32_t  GPIOXDATA4 :  1;               /*!< Output data for pins GPIOn_4                                          */
      __IO uint32_t  GPIOXDATA5 :  1;               /*!< Output data for pins GPIOn_5                                          */
      __IO uint32_t  GPIOXDATA6 :  1;               /*!< Output data for pins GPIOn_6                                          */
      __IO uint32_t  GPIOXDATA7 :  1;               /*!< Output data for pins GPIOn_7                                          */
      __IO uint32_t  GPIOXDATA8 :  1;               /*!< Output data for pins GPIOn_8                                          */
      __IO uint32_t  GPIOXDATA9 :  1;               /*!< Output data for pins GPIOn_9                                          */
      __IO uint32_t  GPIOXDATA10:  1;               /*!< Output data for pins GPIOn_10                                         */
      __IO uint32_t  GPIOXDATA11:  1;               /*!< Output data for pins GPIOn_11                                         */
      __IO uint32_t  GPIOXDATA12:  1;               /*!< Output data for pins GPIOn_12                                         */
      __IO uint32_t  GPIOXDATA13:  1;               /*!< Output data for pins GPIOn_13                                         */
      __IO uint32_t  GPIOXDATA14:  1;               /*!< Output data for pins GPIOn_14                                         */
      __IO uint32_t  GPIOXDATA15:  1;               /*!< Output data for pins GPIOn_15                                         */
    } DATA_b;                                       /*!< BitSize                                                               */
  };
  
  union {
    __I  uint32_t  PIN;                             /*!< GPIO Port X Pin Value.                                                */
    
    struct {
      __I  uint32_t  GPIOXPIN0  :  1;               /*!< Input data for pins GPIOn_0                                           */
      __I  uint32_t  GPIOXPIN1  :  1;               /*!< Input data for pins GPIOn_1                                           */
      __I  uint32_t  GPIOXPIN2  :  1;               /*!< Input data for pins GPIOn_2                                           */
      __I  uint32_t  GPIOXPIN3  :  1;               /*!< Input data for pins GPIOn_3                                           */
      __I  uint32_t  GPIOXPIN4  :  1;               /*!< Input data for pins GPIOn_4                                           */
      __I  uint32_t  GPIOXPIN5  :  1;               /*!< Input data for pins GPIOn_5                                           */
      __I  uint32_t  GPIOXPIN6  :  1;               /*!< Input data for pins GPIOn_6                                           */
      __I  uint32_t  GPIOXPIN7  :  1;               /*!< Input data for pins GPIOn_7                                           */
      __I  uint32_t  GPIOXPIN8  :  1;               /*!< Input data for pins GPIOn_8                                           */
      __I  uint32_t  GPIOXPIN9  :  1;               /*!< Input data for pins GPIOn_9                                           */
      __I  uint32_t  GPIOXPIN10 :  1;               /*!< Input data for pins GPIOn_10                                          */
      __I  uint32_t  GPIOXPIN11 :  1;               /*!< Input data for pins GPIOn_11                                          */
      __I  uint32_t  GPIOXPIN12 :  1;               /*!< Input data for pins GPIOn_12                                          */
      __I  uint32_t  GPIOXPIN13 :  1;               /*!< Input data for pins GPIOn_13                                          */
      __I  uint32_t  GPIOXPIN14 :  1;               /*!< Input data for pins GPIOn_14                                          */
      __I  uint32_t  GPIOXPIN15 :  1;               /*!< Input data for pins GPIOn_15                                          */
    } PIN_b;                                        /*!< BitSize                                                               */
  };
  
  union {
    __IO uint32_t  MODE;                            /*!< GPIO Port X Mode Register.                                            */
    
    struct {
      __IO uint32_t  MODEPIN0   :  2;               /*!< Determine I/O mode of GPIO Port X_0                                   */
      __IO uint32_t  MODEPIN1   :  2;               /*!< Determine I/O mode of GPIO Port X_1                                   */
      __IO uint32_t  MODEPIN2   :  2;               /*!< Determine I/O mode of GPIO Port X_2                                   */
      __IO uint32_t  MODEPIN3   :  2;               /*!< Determine I/O mode of GPIO Port X_3                                   */
      __IO uint32_t  MODEPIN4   :  2;               /*!< Determine I/O mode of GPIO Port X_4                                   */
      __IO uint32_t  MODEPIN5   :  2;               /*!< Determine I/O mode of GPIO Port X_5                                   */
      __IO uint32_t  MODEPIN6   :  2;               /*!< Determine I/O mode of GPIO Port X_6                                   */
      __IO uint32_t  MODEPIN7   :  2;               /*!< Determine I/O mode of GPIO Port X_7                                   */
      __IO uint32_t  MODEPIN8   :  2;               /*!< Determine I/O mode of GPIO Port X_8                                   */
      __IO uint32_t  MODEPIN9   :  2;               /*!< Determine I/O mode of GPIO Port X_9                                   */
      __IO uint32_t  MODEPIN10  :  2;               /*!< Determine I/O mode of GPIO Port X_10                                  */
      __IO uint32_t  MODEPIN11  :  2;               /*!< Determine I/O mode of GPIO Port X_11                                  */
      __IO uint32_t  MODEPIN12  :  2;               /*!< Determine I/O mode of GPIO Port X_12                                  */
      __IO uint32_t  MODEPIN13  :  2;               /*!< Determine I/O mode of GPIO Port X_13                                  */
      __IO uint32_t  MODEPIN14  :  2;               /*!< Determine I/O mode of GPIO Port X_14                                  */
      __IO uint32_t  MODEPIN15  :  2;               /*!< Determine I/O mode of GPIO Port X_15                                  */
    } MODE_b;                                       /*!< BitSize                                                               */
  };
  
  union {
    __IO uint32_t  WMARK;                           /*!< GPIO Port X Data Output Write Mask Register. These bits are
                                                         used to protect the corresponding register bit of DATA register.
                                                          When set, Corresponding bit of DATA register protected.              */
    
    struct {
      __IO uint32_t  GPIOXWMASK0:  1;               /*!< Protect GPIOXDATA0.                                                   */
      __IO uint32_t  GPIOXWMASK1:  1;               /*!< Protect GPIOXDATA1.                                                   */
      __IO uint32_t  GPIOXWMASK2:  1;               /*!< Protect GPIOXDATA2.                                                   */
      __IO uint32_t  GPIOXWMASK3:  1;               /*!< Protect GPIOXDATA3.                                                   */
      __IO uint32_t  GPIOXWMASK4:  1;               /*!< Protect GPIOXDATA4.                                                   */
      __IO uint32_t  GPIOXWMASK5:  1;               /*!< Protect GPIOXDATA5.                                                   */
      __IO uint32_t  GPIOXWMASK6:  1;               /*!< Protect GPIOXDATA6.                                                   */
      __IO uint32_t  GPIOXWMASK7:  1;               /*!< Protect GPIOXDATA7.                                                   */
      __IO uint32_t  GPIOXWMASK8:  1;               /*!< Protect GPIOXDATA8.                                                   */
      __IO uint32_t  GPIOXWMASK9:  1;               /*!< Protect GPIOXDATA9.                                                   */
      __IO uint32_t  GPIOXWMASK10:  1;              /*!< Protect GPIOXDATA10.                                                  */
      __IO uint32_t  GPIOXWMASK11:  1;              /*!< Protect GPIOXDATA11.                                                  */
      __IO uint32_t  GPIOXWMASK12:  1;              /*!< Protect GPIOXDATA12.                                                  */
      __IO uint32_t  GPIOXWMASK13:  1;              /*!< Protect GPIOXDATA13.                                                  */
      __IO uint32_t  GPIOXWMASK14:  1;              /*!< Protect GPIOXDATA14.                                                  */
      __IO uint32_t  GPIOXWMASK15:  1;              /*!< Protect GPIOXDATA15.                                                  */
    } WMARK_b;                                      /*!< BitSize                                                               */
  };
  __I  uint32_t  RESERVED0;
  
  union {
    __IO uint32_t  IEENS0;                          /*!< GPIO Port X Interrupt Enable Set 0 Register.                          */
    
    struct {
      __IO uint32_t  GPIOXIEENS0:  3;               /*!< This register is used to enable the interrupt for GPIO PortX_0.       */
           uint32_t             :  1;
      __IO uint32_t  GPIOXIEENS1:  3;               /*!< This register is used to enable the interrupt for GPIO PortX_1.       */
           uint32_t             :  1;
      __IO uint32_t  GPIOXIEENS2:  3;               /*!< This register is used to enable the interrupt for GPIO PortX_2.       */
           uint32_t             :  1;
      __IO uint32_t  GPIOXIEENS3:  3;               /*!< This register is used to enable the interrupt for GPIO PortX_3.       */
           uint32_t             :  1;
      __IO uint32_t  GPIOXIEENS4:  3;               /*!< This register is used to enable the interrupt for GPIO PortX_4.       */
           uint32_t             :  1;
      __IO uint32_t  GPIOXIEENS5:  3;               /*!< This register is used to enable the interrupt for GPIO PortX_5.       */
           uint32_t             :  1;
      __IO uint32_t  GPIOXIEENS6:  3;               /*!< This register is used to enable the interrupt for GPIO PortX_6.       */
           uint32_t             :  1;
      __IO uint32_t  GPIOXIEENS7:  3;               /*!< This register is used to enable the interrupt for GPIO PortX_7.       */
    } IEENS0_b;                                     /*!< BitSize                                                               */
  };
  
  union {
    __IO uint32_t  IEENS1;                          /*!< GPIO Port X Interrupt Enable Set 1 Register.                          */
    
    struct {
      __IO uint32_t  GPIOXIEENS8:  3;               /*!< This register is used to enable the interrupt for GPIO PortX_8.       */
           uint32_t             :  1;
      __IO uint32_t  GPIOXIEENS9:  3;               /*!< This register is used to enable the interrupt for GPIO PortX_9.       */
           uint32_t             :  1;
      __IO uint32_t  GPIOXIEENS10:  3;              /*!< This register is used to enable the interrupt for GPIO PortX_10.      */
           uint32_t             :  1;
      __IO uint32_t  GPIOXIEENS11:  3;              /*!< This register is used to enable the interrupt for GPIO PortX_11.      */
           uint32_t             :  1;
      __IO uint32_t  GPIOXIEENS12:  3;              /*!< This register is used to enable the interrupt for GPIO PortX_12.      */
           uint32_t             :  1;
      __IO uint32_t  GPIOXIEENS13:  3;              /*!< This register is used to enable the interrupt for GPIO PortX_13.      */
           uint32_t             :  1;
      __IO uint32_t  GPIOXIEENS14:  3;              /*!< This register is used to enable the interrupt for GPIO PortX_14.      */
           uint32_t             :  1;
      __IO uint32_t  GPIOXIEENS15:  3;              /*!< This register is used to enable the interrupt for GPIO PortX_15.      */
    } IEENS1_b;                                     /*!< BitSize                                                               */
  };
  
  union {
    __IO uint32_t  IDBEN;                           /*!< GPIO Port X Interrupt De-Bounce Enabe Register.                       */
    
    struct {
      __IO uint32_t  GPIOXIDBEN : 16;               /*!< This is used to enable the de-bounce function for each corresponding
                                                         bit.                                                                  */
    } IDBEN_b;                                      /*!< BitSize                                                               */
  };
  
  union {
    __IO uint32_t  IDBCU;                           /*!< GPIO Port X Interrupt De-Bounce Counter Register.                     */
    
    struct {
      __IO uint32_t  GPIOXIDBCU : 16;               /*!< De-bounce counter.                                                    */
    } IDBCU_b;                                      /*!< BitSize                                                               */
  };
  
  union {
    __IO uint32_t  IEST;                            /*!< GPIO Port X Interrupt Status Register.                                */
    
    struct {
      __IO uint32_t  GPIOXIEST  : 16;               /*!< When Set, Corresponding pin interrupt has met the requirements.       */
    } IEST_b;                                       /*!< BitSize                                                               */
  };
  
  union {
    __O  uint32_t  IESTC;                           /*!< GPIO Port X Interrupt Status Clear Register.                          */
    
    struct {
      __O  uint32_t  GPIOXIESTC : 16;               /*!< When Set, Corresponding interrupt is cleared. (only write)            */
    } IESTC_b;                                      /*!< BitSize                                                               */
  };
  
  union {
    __IO uint32_t  TADC0S0;                         /*!< GPIO Port X Trigger ADC 0 Set 0 Register.                             */
    
    struct {
      __IO uint32_t  GPIOXTADCS0:  2;               /*!< This register is used to trigger the ADC for GPIO PortX_0.            */
           uint32_t             :  2;
      __IO uint32_t  GPIOXTADCS1:  2;               /*!< This register is used to trigger the ADC for GPIO PortX_1.            */
           uint32_t             :  2;
      __IO uint32_t  GPIOXTADCS2:  2;               /*!< This register is used to trigger the ADC for GPIO PortX_2.            */
           uint32_t             :  2;
      __IO uint32_t  GPIOXTADCS3:  2;               /*!< This register is used to trigger the ADC for GPIO PortX_3.            */
           uint32_t             :  2;
      __IO uint32_t  GPIOXTADCS4:  2;               /*!< This register is used to trigger the ADC for GPIO PortX_4.            */
           uint32_t             :  2;
      __IO uint32_t  GPIOXTADCS5:  2;               /*!< This register is used to trigger the ADC for GPIO PortX_5.            */
           uint32_t             :  2;
      __IO uint32_t  GPIOXTADCS6:  2;               /*!< This register is used to trigger the ADC for GPIO PortX_6.            */
           uint32_t             :  2;
      __IO uint32_t  GPIOXTADCS7:  2;               /*!< This register is used to trigger the ADC for GPIO PortX_7.            */
    } TADC0S0_b;                                    /*!< BitSize                                                               */
  };
  
  union {
    __IO uint32_t  TADC0S1;                         /*!< GPIO Port X Trigger ADC 0 Set 1 Register.                             */
    
    struct {
      __IO uint32_t  GPIOXTADCS8:  2;               /*!< This register is used to trigger the ADC for GPIO PortX_8.            */
           uint32_t             :  2;
      __IO uint32_t  GPIOXTADCS9:  2;               /*!< This register is used to trigger the ADC for GPIO PortX_9.            */
           uint32_t             :  2;
      __IO uint32_t  GPIOXTADCS10:  2;              /*!< This register is used to trigger the ADC for GPIO PortX_10.           */
           uint32_t             :  2;
      __IO uint32_t  GPIOXTADCS11:  2;              /*!< This register is used to trigger the ADC for GPIO PortX_11.           */
           uint32_t             :  2;
      __IO uint32_t  GPIOXTADCS12:  2;              /*!< This register is used to trigger the ADC for GPIO PortX_12.           */
           uint32_t             :  2;
      __IO uint32_t  GPIOXTADCS13:  2;              /*!< This register is used to trigger the ADC for GPIO PortX_13.           */
           uint32_t             :  2;
      __IO uint32_t  GPIOXTADCS14:  2;              /*!< This register is used to trigger the ADC for GPIO PortX_14.           */
           uint32_t             :  2;
      __IO uint32_t  GPIOXTADCS15:  2;              /*!< This register is used to trigger the ADC for GPIO PortX_15.           */
    } TADC0S1_b;                                    /*!< BitSize                                                               */
  };
  __I  uint32_t  RESERVED1[2];
  
  union {
    __IO uint32_t  MF0;                             /*!< GPIO Port X Multiple Function 0 Register.                             */
    
    struct {
      __IO uint32_t  PORT_0     :  4;               /*!< This register determine the PortX_0 function.                         */
      __IO uint32_t  PORT_1     :  4;               /*!< This register determine the PortX_1 function.                         */
      __IO uint32_t  PORT_2     :  4;               /*!< This register determine the PortX_2 function.                         */
      __IO uint32_t  PORT_3     :  4;               /*!< This register determine the PortX_3 function.                         */
      __IO uint32_t  PORT_4     :  4;               /*!< This register determine the PortX_4 function.                         */
      __IO uint32_t  PORT_5     :  4;               /*!< This register determine the PortX_5 function.                         */
      __IO uint32_t  PORT_6     :  4;               /*!< This register determine the PortX_6 function.                         */
      __IO uint32_t  PORT_7     :  4;               /*!< This register determine the PortX_7 function.                         */
    } MF0_b;                                        /*!< BitSize                                                               */
  };
  
  union {
    __IO uint32_t  MF1;                             /*!< GPIO Port X Multiple Function 1 Register. Only for GPIO0, GPIO1
                                                         and GPIO3.                                                            */
    
    struct {
      __IO uint32_t  PORT_8     :  4;               /*!< This register determine the PortX_8 function.                         */
      __IO uint32_t  PORT_9     :  4;               /*!< This register determine the PortX_9 function.                         */
      __IO uint32_t  PORT_10    :  4;               /*!< This register determine the PortX_10 function.                        */
      __IO uint32_t  PORT_11    :  4;               /*!< This register determine the PortX_11 function.                        */
      __IO uint32_t  PORT_12    :  4;               /*!< This register determine the PortX_12 function.                        */
      __IO uint32_t  PORT_13    :  4;               /*!< This register determine the PortX_13 function.                        */
      __IO uint32_t  PORT_14    :  4;               /*!< This register determine the PortX_14 function.                        */
      __IO uint32_t  PORT_15    :  4;               /*!< This register determine the PortX_15 function.                        */
    } MF1_b;                                        /*!< BitSize                                                               */
  };
} OB_GPIO_Type;


/* ================================================================================ */
/* ================                     SYSCON                     ================ */
/* ================================================================================ */


/**
  * @brief System configuration (SYSCON)
  */

typedef struct {                                    /*!< SYSCON Structure                                                      */
  __I  uint32_t  RESERVED0[4];
  
  union {
    __IO uint32_t  CCLKCFG;                         /*!< CPU Clock Configuration Register.                                     */
    
    struct {
      __IO uint32_t  CCLKSEL    :  8;               /*!< Selects the divide value from creating the CPU clock (CCLK)
                                                         from the multiplexer output (EXTL/PLLOUT/HIRC). The clock is
                                                          divided by CCLKSEL to produce the CPU clock.note : the rate
                                                          would always be greater than the maximum allowed CPU clock.          */
      __IO uint32_t  MCUSRCSEL  :  2;               /*!< MCU clock source select.                                              */
      __IO uint32_t  CPUWAITCNT :  2;               /*!< CPU wait count. Limited to the flash data read access time.
                                                         If the CPU clock period is less than the access time, the user
                                                          set wait counter to read rom core.                                   */
           uint32_t             :  4;
      __IO uint32_t  DPLL_PWME  :  1;               /*!< PWM clock source select.                                              */
           uint32_t             : 14;
      __IO uint32_t  SYSCLKPROBEE:  1;              /*!< System clock probe enable.                                            */
    } CCLKCFG_b;                                    /*!< BitSize                                                               */
  };
  __I  uint32_t  RESERVED1;
  
  union {
    __IO uint32_t  PCLKSEL0;                        /*!< Peripheral Clock Selection 0 Register.                                */
    
    struct {
      __IO uint32_t  CS_SPI     :  2;               /*!< Clock selection for peripheral SPI.                                   */
      __IO uint32_t  CS_CORDIC  :  2;               /*!< Clock selection for peripheral CORDIC.                                */
      __IO uint32_t  CS_QEI     :  2;               /*!< Clock selection for peripheral QEI.                                   */
      __IO uint32_t  CS_RTC     :  2;               /*!< Clock selection for peripheral RTC.                                   */
      __IO uint32_t  CS_PDMA    :  2;               /*!< Clock selection for peripheral PDMA.                                  */
      __IO uint32_t  CS_MDU     :  2;               /*!< Clock selection for peripheral MDU.                                   */
      __IO uint32_t  CS_ADC0    :  2;               /*!< Clock selection for peripheral ADC 0.                                 */
      __IO uint32_t  CS_ADC1    :  2;               /*!< Clock selection for peripheral ADC 1.                                 */
      __IO uint32_t  CS_UART0   :  2;               /*!< Clock selection for peripheral UART0.                                 */
      __IO uint32_t  CS_WWDT    :  2;               /*!< Clock selection for peripheral WWDT.                                  */
      __IO uint32_t  CS_I2C0    :  2;               /*!< Clock selection for peripheral I2C 0.                                 */
      __IO uint32_t  CS_UART1   :  2;               /*!< Clock selection for peripheral UART 1.                                */
      __IO uint32_t  CS_PWM     :  2;               /*!< Clock selection for peripheral PWM.                                   */
      __IO uint32_t  CS_I2C1    :  2;               /*!< Clock selection for peripheral I2C 1.                                 */
      __IO uint32_t  CS_CT16B0  :  2;               /*!< Clock selection for peripheral Count/Timer 16 bit 0.                  */
      __IO uint32_t  CS_CT16B1  :  2;               /*!< Clock selection for peripheral Count/Timer 16 bit 1.                  */
    } PCLKSEL0_b;                                   /*!< BitSize                                                               */
  };
  
  union {
    __IO uint32_t  PCLKSEL1;                        /*!< Peripheral Clock Selection 1 Register.                                */
    
    struct {
      __IO uint32_t  CS_CT32B0  :  2;               /*!< Clock selection for peripheral Count/Timer 32 bit 0.                  */
      __IO uint32_t  CS_CT32B1  :  2;               /*!< Clock selection for peripheral Count/Timer 32 bit 1.                  */
    } PCLKSEL1_b;                                   /*!< BitSize                                                               */
  };
  
  union {
    __IO uint32_t  PCGP;                            /*!< Power/Clock Gating for Peripherals Register.                          */
    
    struct {
      __IO uint32_t  PCSPI      :  1;               /*!< SPI power/clock control bit.                                          */
      __IO uint32_t  PC_CORDIC  :  1;               /*!< CORDIC power/clock control bit.                                       */
      __IO uint32_t  PC_QEI     :  1;               /*!< QEI power/clock control bit.                                          */
      __IO uint32_t  PC_RTC     :  1;               /*!< RTC power/clock control bit.                                          */
           uint32_t             :  1;
      __IO uint32_t  PC_MDU     :  1;               /*!< MDU power/clock control bit.                                          */
      __IO uint32_t  PC_ADC0    :  1;               /*!< A/D converter (ADC0) power/clock control bit.                         */
      __IO uint32_t  PC_ADC1    :  1;               /*!< A/D converter (ADC1) power/clock control bit.                         */
      __IO uint32_t  PC_UART0   :  1;               /*!< UART 0 power/clock control bit.                                       */
      __IO uint32_t  PC_WWDT    :  1;               /*!< Window watchdog Timer power/clock control bit.                        */
      __IO uint32_t  PC_I2C0    :  1;               /*!< I2C power/clock control bit.                                          */
      __IO uint32_t  PC_UART1   :  1;               /*!< UART 1 power/clock control bit.                                       */
      __IO uint32_t  PC_PWM     :  1;               /*!< Power Control PWM power/clock control bit.                            */
      __IO uint32_t  PC_I2C1    :  1;               /*!< The I2C 1 interface power/clock control bit.                          */
      __IO uint32_t  PC_CT16B0  :  1;               /*!< Count/Timer 16 bit 0 power/clock control bit.                         */
      __IO uint32_t  PC_CT16B1  :  1;               /*!< Count/Timer 16 bit 1 power/clock control bit.                         */
      __IO uint32_t  PC_CT32B0  :  1;               /*!< Count/Timer 32 bit 0 power/clock control bit.                         */
      __IO uint32_t  PC_CT32B1  :  1;               /*!< Count/Timer 32 bit 1 power/clock control bit.                         */
    } PCGP_b;                                       /*!< BitSize                                                               */
  };
  
  union {
    __IO uint32_t  PCON;                            /*!< Power Mode Control Register.                                          */
    
    struct {
      __IO uint32_t  PM         :  2;               /*!< Power mode control bits which controls entry to the Power-down
                                                         mode..                                                                */
           uint32_t             :  2;
      __IO uint32_t  SMFLAG     :  1;               /*!< Sleep Mode entry flag. Set when the Sleep mode is successfully
                                                         entered. Cleared by software writing a one to this bit.               */
      __IO uint32_t  DSMFLAG    :  1;               /*!< Deep Sleep Mode entry flag. Set when the Deep Sleep mode is
                                                         successfully entered. Cleared by software writing a one to this
                                                          bit.                                                                 */
      __IO uint32_t  PDFLAG     :  1;               /*!< Power-down entry flag. Set when the Power-down mode is successfully
                                                         entered. Cleared by software writing a one to this bit.               */
      __IO uint32_t  DPDFLAG    :  1;               /*!< Deep Power-down entry flag. Set when the Deep Power-down mode
                                                         is successfully entered. Cleared by software writing a one to
                                                          this bit.                                                            */
    } PCON_b;                                       /*!< BitSize                                                               */
  };
  __I  uint32_t  RESERVED2[22];
  
  union {
    __IO uint32_t  DPLLCON;                         /*!< DPLL Control Register.                                                */
    
    struct {
      __IO uint32_t  PLLSRCSEL  :  1;               /*!< DPLL clock source selection.                                          */
      __IO uint32_t  OSC_MEET   :  1;               /*!< OSCMEET                                                               */
      __IO uint32_t  OSCTRIM_FAIL:  1;              /*!< OSCTRIMFAIL                                                           */
      __IO uint32_t  ETSEL      :  1;               /*!< ETSEL                                                                 */
      __IO uint32_t  CREF_DIV_SEL:  4;              /*!< DPLL reference divide clock.                                          */
      __IO uint32_t  OSC_DIV_SEL:  6;               /*!< DPLL(DPLL_FREQ_SEL[5:0]) divide clock.0~2 : DPLL(DPLL_FREQ_SEL
                                                         [5:0]) divide 2. Other: DPLL(DPLL_FREQ_SEL [5:0]) divide DPLL_DIV_SEL. */
           uint32_t             :  2;
      __IO uint32_t  DPLL_FREQ_SEL:  6;             /*!< DPLL Frequency Select.                                                */
           uint32_t             :  2;
      __IO uint32_t  A_IN       :  4;               /*!< A IN                                                                  */
      __IO uint32_t  B_IN       :  4;               /*!< B IN                                                                  */
    } DPLLCON_b;                                    /*!< BitSize                                                               */
  };
  
  union {
    __IO uint32_t  DPLL2CON;                        /*!< DPLL2 Control Register.                                               */
    
    struct {
           uint32_t             :  8;
      __IO uint32_t  PWM_DIV_SEL:  3;               /*!< DPLL(DPLL_FREQ_SEL[5:0]) divide clock.0 - DPLL(DPLL_FREQ_SEL
                                                         [5:0]) divide 2 for PWM1 - DPLL(DPLL_FREQ_SEL [5:0]) divide
                                                          2 for PWM2 - DPLL(DPLL_FREQ_SEL [5:0]) divide 2 for PWM3 - DPLL(DPLL_FREQ_
                                                         SEL [5:0]) divide 3 for PWM4 - DPLL(DPLL_FREQ_SEL [5:0]) divide
                                                          4 for PWM5 - DPLL(DPLL_FREQ_SEL [5:0]) divide 5 for PWM6 - DPLL(DPLL_FREQ_
                                                         SEL [5:0]) divide 6 for PWM7 - DPLL(DPLL_FREQ_SEL [5:0]) divide
                                                          7 for PWM                                                            */
    } DPLL2CON_b;                                   /*!< BitSize                                                               */
  };
  __I  uint32_t  RESERVED3[6];
  
  union {
    __IO uint32_t  CPUFLSHCTR0;                     /*!< CPU/FLASH Control 0 Register.                                         */
    
    struct {
      __IO uint32_t  WTSWD_SEL  :  4;               /*!< Wait time for Serial-Wire Debug (SWD) select.0x0, 0xF: 200K
                                                         clock stable count = 2^11 cycle (default)0x1: 200K clock stable
                                                          count = 2^12 cycle0x2: 200K clock stable count = 2^13 cycle0x3:
                                                          200K clock stable count = 2^14 cycle0x4: 200K clock stable count
                                                          = 2^15 cycleother: 200K clock stable count = 2^16 cycle              */
    } CPUFLSHCTR0_b;                                /*!< BitSize                                                               */
  };
  
  union {
    __IO uint32_t  CPUFLSHCTR1;                     /*!< CPU/FLASH Control 1 Register.                                         */
    
    struct {
      __IO uint32_t  OSC24M_TR  :  7;               /*!< Embosc trim value. default = 100_0000 and FW can read/write
                                                         value                                                                 */
    } CPUFLSHCTR1_b;                                /*!< BitSize                                                               */
  };
  
  union {
    __IO uint32_t  CPUFLSHCTR2;                     /*!< CPU/FLASH Control 1 Register.                                         */
    
    struct {
      __IO uint32_t  BGCAL      :  5;               /*!< Bandgap value, default = 1_1000 and FW can read/write value.          */
    } CPUFLSHCTR2_b;                                /*!< BitSize                                                               */
  };
  __I  uint32_t  RESERVED4;
  
  union {
    __IO uint32_t  CPUCHIPCTR0;                     /*!< CPU/CHIP Control 0 Register.                                          */
    
    struct {
           uint32_t             :  1;
      __IO uint32_t  PADE_SWD   :  1;               /*!< Pad Enable SWD (SWD : Serial Wire Debug)SWD_DATA / SWD_CLK (chip
                                                         pad) can be set in two modes. GPIO or Serial Wire Debug function.     */
      __IO uint32_t  PADE_KXTAL :  1;               /*!< PAD Enable KHz XTALKXTAL_IN / KXTAL_OUT (chip pad) can be set
                                                         in two modes, GPIO or 32KHz XTAL function.                            */
      __IO uint32_t  PADE_MXTAL :  1;               /*!< PAD Enable MHz XTALMXTAL_IN / MXTAL_OUT (chip pad) can be set
                                                         in two modes, GPIO or MHz XTAL function.                              */
      __IO uint32_t  XTGS       :  1;               /*!< XTAL gain select. (for MXTAL function, default 1)                     */
           uint32_t             :  7;
      __IO uint32_t  DPLL_SEL   :  4;               /*!< DPLL clock stable count select ( depend on LIRC = 20K clock
                                                         )                                                                     */
      __IO uint32_t  MXTAL_SEL  :  4;               /*!< MXTAL clock stable count select ( depend on MXTAL clock )             */
      __IO uint32_t  KXTAL_RDY  :  1;               /*!< KXTAL_RDY                                                             */
      __IO uint32_t  MXTAL_RDY  :  1;               /*!< MXTAL_RDY                                                             */
      __IO uint32_t  DPLL_RDY   :  1;               /*!< DPLL_RDY                                                              */
      __IO uint32_t  HIRC_RDY   :  1;               /*!< HIRC_RDY                                                              */
      __IO uint32_t  LIRC_RDY   :  1;               /*!< LIRC_RDY                                                              */
    } CPUCHIPCTR0_b;                                /*!< BitSize                                                               */
  };
  
  union {
    __IO uint32_t  CPUCHIPCTR1;                     /*!< CPU/CHIP Control 1 Register.                                          */
    
    struct {
      __IO uint32_t  CKFKXSEL   :  2;               /*!< Clock filter for KXTAL select (Deglitch time)                         */
      __IO uint32_t  CKFMXSEL   :  2;               /*!< Clock filter for MXTAL select (Deglitch time)                         */
    } CPUCHIPCTR1_b;                                /*!< BitSize                                                               */
  };
  __I  uint32_t  RESERVED5[2];
  
  union {
    __IO uint32_t  LVRLVICTRSTA0;                   /*!< LVR/LVI Control/Status 0 Control Register.                            */
    
    struct {
      __IO uint32_t  PD_LVR     :  1;               /*!< Power Down LVR function.                                              */
      __IO uint32_t  PD_LVR_LP  :  1;               /*!< Power Down LVR_LP function. (LVR_LP: LVR low power)                   */
      __IO uint32_t  PD_LVRLP_INT:  1;              /*!< Power Down LVRLP_INT function. ( LVRLP_INT: LVR low power internal)   */
      __IO uint32_t  PD_LVI     :  1;               /*!< Power Down LVI function                                               */
      __IO uint32_t  LVI_SEL    :  2;               /*!< LVI selection.                                                        */
      __IO uint32_t  LVRRSTE    :  1;               /*!< LVR Reset.                                                            */
      __IO uint32_t  LVRLPRSTE  :  1;               /*!< LVRLP Reset.                                                          */
           uint32_t             :  8;
      __IO uint32_t  LVR        :  1;               /*!< analog status. (high active)                                          */
      __IO uint32_t  LVR_LP     :  1;               /*!< analog status. (high active)                                          */
      __IO uint32_t  LVRLP_INT  :  1;               /*!< analog status. (high active)                                          */
      __IO uint32_t  LVI        :  1;               /*!< analog status. (high active)                                          */
      __IO uint32_t  LVIINT     :  1;               /*!< LVI interrupt.                                                        */
           uint32_t             :  1;
      __IO uint32_t  LVRRSTFLAG :  1;               /*!< LVR Reset Flag.                                                       */
      __IO uint32_t  LVRLPRSTFLAG:  1;              /*!< LVRLP Reset Flag.                                                     */
    } LVRLVICTRSTA0_b;                              /*!< BitSize                                                               */
  };
  __I  uint32_t  RESERVED6[3];
  
  union {
    __IO uint32_t  USRFLSHCTR0;                     /*!< USR/Flash Control 0 Register                                          */
    
    struct {
           uint32_t             : 16;
      __IO uint32_t  PADE_RESET :  8;               /*!< Pad Enable reset.RESET (chip pad) can be set in two modes. GPIO
                                                         or RESET function.Flash data = 0x5A, or firmware write data
                                                          = 0x5A. GPIO function enable. (write bit23 : bit16)                  */
      __IO uint32_t  EQADDR     :  1;               /*!< EQADDR                                                                */
      __IO uint32_t  BRANCH     :  1;               /*!< BRANCH                                                                */
      __IO uint32_t  PREDICT    :  1;               /*!< PREDICT                                                               */
      __IO uint32_t  PREFETCH_BUFF:  1;             /*!< PREFETCH_BUFF                                                         */
      __IO uint32_t  PREFETCH_MODE:  1;             /*!< PREFETCH_MODE                                                         */
           uint32_t             :  2;
      __IO uint32_t  MCUACC_PBUFEN:  1;             /*!< MCUACC_PBUFEN1. Reload from flash ( value inverter )2. FW write
                                                         ( no inverter )                                                       */
    } USRFLSHCTR0_b;                                /*!< BitSize                                                               */
  };
  __I  uint32_t  RESERVED7[9];
  
  union {
    __IO uint32_t  FUNCIPTRIM;                      /*!< Function/IP Trim Register.                                            */
    
    struct {
      __IO uint32_t  TRIM_VALUE :  8;               /*!< CHECK_NUM = 0x0001, OSC24M_TR[6:0] = TRIM_VALUE[6:0]. CHECK_NUM
                                                         = 0x0002, BGCAL[12:8] = TRIM_VALUE[4:0].                              */
           uint32_t             :  8;
      __IO uint32_t  CHECK_NUM  : 16;               /*!< Check number.                                                         */
    } FUNCIPTRIM_b;                                 /*!< BitSize                                                               */
  };
  
  union {
    __IO uint32_t  FUNCIPTM;                        /*!< Function/IP Test Mode Register.                                       */
    
    struct {
           uint32_t             :  1;
      __IO uint32_t  RAGSEL     :  2;               /*!< Osc test mode (trim) range select. detect 512 cycles.                 */
      __IO uint32_t  RAGSELP    :  1;               /*!< Osc test mode (trim) range select. detect 512 cycle                   */
           uint32_t             :  3;
      __IO uint32_t  ENABLE     :  1;               /*!< Osc test mode (trim).                                                 */
      __IO uint32_t  DPLLDBG    :  1;               /*!< DPLL debug.                                                           */
    } FUNCIPTM_b;                                   /*!< BitSize                                                               */
  };
} OB_SYSCON_Type;


/* ================================================================================ */
/* ================                     CORDIC                     ================ */
/* ================================================================================ */


/**
  * @brief Coordinate Rotation Digital Computer (CORDIC)
  */

typedef struct {                                    /*!< CORDIC Structure                                                      */
  
  union {
    __IO uint32_t  CORDIC_CTR;                      /*!< CORDIC Control Register.                                              */
    
    struct {
      __IO uint32_t  TRIG       :  1;               /*!< CORDIC function trigger.                                              */
      __IO uint32_t  MODE       :  1;               /*!< CORDIC algorithm has two types of computing modes Vector rotation
                                                         and vector translation.                                               */
      __IO uint32_t  CSYS       :  2;               /*!< CORDIC algorithm perform rotation in circular, linear and hyperbolic
                                                         coordinate systems. The unified formulation includes a new variable
                                                          m, which is assigned different values for different coordinate
                                                          systems.                                                             */
      __IO uint32_t  PARASEL    :  1;               /*!< Parameter select.                                                     */
      __IO uint32_t  CCPSEL     :  1;               /*!< Circular parameter select.                                            */
           uint32_t             : 18;
      __IO uint32_t  ITCNT      :  4;               /*!< Iteration count, the default value is 8, write only.                  */
      __IO uint32_t  TCNT_KEY   :  4;               /*!< Iteration count key.                                                  */
    } CORDIC_CTR_b;                                 /*!< BitSize                                                               */
  };
  __I  uint32_t  RESERVED0[3];
  
  union {
    __IO uint32_t  CORDIC_X;                        /*!< CORDIC Data Format Register.                                          */
    
    struct {
      __IO uint32_t  CORDIC_X   : 31;               /*!< Data value                                                            */
      __IO uint32_t  SIGN_X     :  1;               /*!< The sign bit is as simple as it gets. Flipping the value of
                                                         this bit flips the sign of the number.                                */
    } CORDIC_X_b;                                   /*!< BitSize                                                               */
  };
  
  union {
    __IO uint32_t  CORDIC_Y;                        /*!< CORDIC Data Format Register.                                          */
    
    struct {
      __IO uint32_t  CORDIC_Y   : 31;               /*!< Data value                                                            */
      __IO uint32_t  SIGN_Y     :  1;               /*!< The sign bit is as simple as it gets. Flipping the value of
                                                         this bit flips the sign of the number.                                */
    } CORDIC_Y_b;                                   /*!< BitSize                                                               */
  };
  
  union {
    __IO uint32_t  CORDIC_Z;                        /*!< CORDIC Data Format Register.                                          */
    
    struct {
      __IO uint32_t  CORDIC_Z   : 31;               /*!< Data value                                                            */
      __IO uint32_t  SIGN_Z     :  1;               /*!< The sign bit is as simple as it gets. Flipping the value of
                                                         this bit flips the sign of the number.                                */
    } CORDIC_Z_b;                                   /*!< BitSize                                                               */
  };
  __I  uint32_t  RESERVED1[9];
  
  union {
    __I  uint32_t  ARCTAN_ANGLE_0;                  /*!< 45                                                                    */
    
    struct {
      __I  uint32_t  ARCTAN_ANGLE_0: 32;            /*!< 45                                                                    */
    } ARCTAN_ANGLE_0_b;                             /*!< BitSize                                                               */
  };
  
  union {
    __I  uint32_t  ARCTAN_ANGLE_1;                  /*!< 26.5650511770780                                                      */
    
    struct {
      __I  uint32_t  ARCTAN_ANGLE_1: 32;            /*!< 26.5650511770780                                                      */
    } ARCTAN_ANGLE_1_b;                             /*!< BitSize                                                               */
  };
  
  union {
    __I  uint32_t  ARCTAN_ANGLE_2;                  /*!< 14.0362434679265                                                      */
    
    struct {
      __I  uint32_t  ARCTAN_ANGLE_2: 32;            /*!< 14.0362434679265                                                      */
    } ARCTAN_ANGLE_2_b;                             /*!< BitSize                                                               */
  };
  
  union {
    __I  uint32_t  ARCTAN_ANGLE_3;                  /*!< 7.12501634890180                                                      */
    
    struct {
      __I  uint32_t  ARCTAN_ANGLE_3: 32;            /*!< 7.12501634890180                                                      */
    } ARCTAN_ANGLE_3_b;                             /*!< BitSize                                                               */
  };
  
  union {
    __I  uint32_t  ARCTAN_ANGLE_4;                  /*!< 3.57633437499735                                                      */
    
    struct {
      __I  uint32_t  ARCTAN_ANGLE_4: 32;            /*!< 3.57633437499735                                                      */
    } ARCTAN_ANGLE_4_b;                             /*!< BitSize                                                               */
  };
  
  union {
    __I  uint32_t  ARCTAN_ANGLE_5;                  /*!< 1.78991060824607                                                      */
    
    struct {
      __I  uint32_t  ARCTAN_ANGLE_5: 32;            /*!< 1.78991060824607                                                      */
    } ARCTAN_ANGLE_5_b;                             /*!< BitSize                                                               */
  };
  
  union {
    __I  uint32_t  ARCTAN_ANGLE_6;                  /*!< 0.895173710211074                                                     */
    
    struct {
      __I  uint32_t  ARCTAN_ANGLE_6: 32;            /*!< 0.895173710211074                                                     */
    } ARCTAN_ANGLE_6_b;                             /*!< BitSize                                                               */
  };
  
  union {
    __I  uint32_t  ARCTAN_ANGLE_7;                  /*!< 0.447614170860553                                                     */
    
    struct {
      __I  uint32_t  ARCTAN_ANGLE_7: 32;            /*!< 0.447614170860553                                                     */
    } ARCTAN_ANGLE_7_b;                             /*!< BitSize                                                               */
  };
  
  union {
    __I  uint32_t  ARCTAN_ANGLE_8;                  /*!< 0.223810500368538                                                     */
    
    struct {
      __I  uint32_t  ARCTAN_ANGLE_8: 32;            /*!< 0.223810500368538                                                     */
    } ARCTAN_ANGLE_8_b;                             /*!< BitSize                                                               */
  };
  
  union {
    __I  uint32_t  ARCTAN_ANGLE_9;                  /*!< 0.111905677066207                                                     */
    
    struct {
      __I  uint32_t  ARCTAN_ANGLE_9: 32;            /*!< 0.111905677066207                                                     */
    } ARCTAN_ANGLE_9_b;                             /*!< BitSize                                                               */
  };
  
  union {
    __I  uint32_t  ARCTAN_ANGLE_A;                  /*!< 0.0559528918938037                                                    */
    
    struct {
      __I  uint32_t  ARCTAN_ANGLE_A: 32;            /*!< 0.0559528918938037                                                    */
    } ARCTAN_ANGLE_A_b;                             /*!< BitSize                                                               */
  };
  
  union {
    __I  uint32_t  ARCTAN_ANGLE_B;                  /*!< 0.0279764526170037                                                    */
    
    struct {
      __I  uint32_t  ARCTAN_ANGLE_B: 32;            /*!< 0.0279764526170037                                                    */
    } ARCTAN_ANGLE_B_b;                             /*!< BitSize                                                               */
  };
  
  union {
    __I  uint32_t  ARCTAN_ANGLE_C;                  /*!< 0.0139882271422650                                                    */
    
    struct {
      __I  uint32_t  ARCTAN_ANGLE_C: 32;            /*!< 0.0139882271422650                                                    */
    } ARCTAN_ANGLE_C_b;                             /*!< BitSize                                                               */
  };
  
  union {
    __I  uint32_t  ARCTAN_ANGLE_D;                  /*!< 0.00699411367535292                                                   */
    
    struct {
      __I  uint32_t  ARCTAN_ANGLE_D: 32;            /*!< 0.00699411367535292                                                   */
    } ARCTAN_ANGLE_D_b;                             /*!< BitSize                                                               */
  };
  
  union {
    __I  uint32_t  ARCTAN_ANGLE_E;                  /*!< 0.00349705685070401                                                   */
    
    struct {
      __I  uint32_t  ARCTAN_ANGLE_E: 32;            /*!< 0.00349705685070401                                                   */
    } ARCTAN_ANGLE_E_b;                             /*!< BitSize                                                               */
  };
  
  union {
    __I  uint32_t  ARCTAN_ANGLE_F;                  /*!< 0.00174852842698045                                                   */
    
    struct {
      __I  uint32_t  ARCTAN_ANGLE_F: 32;            /*!< 0.00174852842698045                                                   */
    } ARCTAN_ANGLE_F_b;                             /*!< BitSize                                                               */
  };
  
  union {
    __I  uint32_t  ARCTAN_RADIUS_0;                 /*!< 0.785398163397448                                                     */
    
    struct {
      __I  uint32_t  ARCTAN_RADIUS_0: 32;           /*!< 0.785398163397448                                                     */
    } ARCTAN_RADIUS_0_b;                            /*!< BitSize                                                               */
  };
  
  union {
    __I  uint32_t  ARCTAN_RADIUS_1;                 /*!< 0.463647609000806                                                     */
    
    struct {
      __I  uint32_t  ARCTAN_RADIUS_1: 32;           /*!< 0.463647609000806                                                     */
    } ARCTAN_RADIUS_1_b;                            /*!< BitSize                                                               */
  };
  
  union {
    __I  uint32_t  ARCTAN_RADIUS_2;                 /*!< 0.244978663126864                                                     */
    
    struct {
      __I  uint32_t  ARCTAN_RADIUS_2: 32;           /*!< 0.244978663126864                                                     */
    } ARCTAN_RADIUS_2_b;                            /*!< BitSize                                                               */
  };
  
  union {
    __I  uint32_t  ARCTAN_RADIUS_3;                 /*!< 0.124354994546761                                                     */
    
    struct {
      __I  uint32_t  ARCTAN_RADIUS_3: 32;           /*!< 0.124354994546761                                                     */
    } ARCTAN_RADIUS_3_b;                            /*!< BitSize                                                               */
  };
  
  union {
    __I  uint32_t  ARCTAN_RADIUS_4;                 /*!< 0.0624188099959574                                                    */
    
    struct {
      __I  uint32_t  ARCTAN_RADIUS_4: 32;           /*!< 0.0624188099959574                                                    */
    } ARCTAN_RADIUS_4_b;                            /*!< BitSize                                                               */
  };
  
  union {
    __I  uint32_t  ARCTAN_RADIUS_5;                 /*!< 0.0312398334302683                                                    */
    
    struct {
      __I  uint32_t  ARCTAN_RADIUS_5: 32;           /*!< 0.0312398334302683                                                    */
    } ARCTAN_RADIUS_5_b;                            /*!< BitSize                                                               */
  };
  
  union {
    __I  uint32_t  ARCTAN_RADIUS_6;                 /*!< 0.0156237286204768                                                    */
    
    struct {
      __I  uint32_t  ARCTAN_RADIUS_6: 32;           /*!< 0.0156237286204768                                                    */
    } ARCTAN_RADIUS_6_b;                            /*!< BitSize                                                               */
  };
  
  union {
    __I  uint32_t  ARCTAN_RADIUS_7;                 /*!< 0.00781234106010111                                                   */
    
    struct {
      __I  uint32_t  ARCTAN_RADIUS_7: 32;           /*!< 0.00781234106010111                                                   */
    } ARCTAN_RADIUS_7_b;                            /*!< BitSize                                                               */
  };
  
  union {
    __I  uint32_t  ARCTAN_RADIUS_8;                 /*!< 0.00390623013196697                                                   */
    
    struct {
      __I  uint32_t  ARCTAN_RADIUS_8: 32;           /*!< 0.00390623013196697                                                   */
    } ARCTAN_RADIUS_8_b;                            /*!< BitSize                                                               */
  };
  
  union {
    __I  uint32_t  ARCTAN_RADIUS_9;                 /*!< 0.00195312251647882                                                   */
    
    struct {
      __I  uint32_t  ARCTAN_RADIUS_9: 32;           /*!< 0.00195312251647882                                                   */
    } ARCTAN_RADIUS_9_b;                            /*!< BitSize                                                               */
  };
  
  union {
    __I  uint32_t  ARCTAN_RADIUS_A;                 /*!< 0.000976562189559320                                                  */
    
    struct {
      __I  uint32_t  ARCTAN_RADIUS_A: 32;           /*!< 0.000976562189559320                                                  */
    } ARCTAN_RADIUS_A_b;                            /*!< BitSize                                                               */
  };
  
  union {
    __I  uint32_t  ARCTAN_RADIUS_B;                 /*!< 0.000488281211194898                                                  */
    
    struct {
      __I  uint32_t  ARCTAN_RADIUS_B: 32;           /*!< 0.000488281211194898                                                  */
    } ARCTAN_RADIUS_B_b;                            /*!< BitSize                                                               */
  };
  
  union {
    __I  uint32_t  ARCTAN_RADIUS_C;                 /*!< 0.000244140620149362                                                  */
    
    struct {
      __I  uint32_t  ARCTAN_RADIUS_C: 32;           /*!< 0.000244140620149362                                                  */
    } ARCTAN_RADIUS_C_b;                            /*!< BitSize                                                               */
  };
  
  union {
    __I  uint32_t  ARCTAN_RADIUS_D;                 /*!< 0.000122070311893670                                                  */
    
    struct {
      __I  uint32_t  ARCTAN_RADIUS_D: 32;           /*!< 0.000122070311893670                                                  */
    } ARCTAN_RADIUS_D_b;                            /*!< BitSize                                                               */
  };
  
  union {
    __I  uint32_t  ARCTAN_RADIUS_E;                 /*!< 0.000061035156174208                                                  */
    
    struct {
      __I  uint32_t  ARCTAN_RADIUS_E: 32;           /*!< 0.000061035156174208                                                  */
    } ARCTAN_RADIUS_E_b;                            /*!< BitSize                                                               */
  };
  
  union {
    __I  uint32_t  ARCTAN_RADIUS_F;                 /*!< 0.000030517578115526                                                  */
    
    struct {
      __I  uint32_t  ARCTAN_RADIUS_F: 32;           /*!< 0.000030517578115526                                                  */
    } ARCTAN_RADIUS_F_b;                            /*!< BitSize                                                               */
  };
  
  union {
    __I  uint32_t  ARCTANH_0;                       /*!< Infinity                                                              */
    
    struct {
      __I  uint32_t  ARCTANH_0  : 32;               /*!< Infinity                                                              */
    } ARCTANH_0_b;                                  /*!< BitSize                                                               */
  };
  
  union {
    __I  uint32_t  ARCTANH_1;                       /*!< 0.549306144334055                                                     */
    
    struct {
      __I  uint32_t  ARCTANH_1  : 32;               /*!< 0.549306144334055                                                     */
    } ARCTANH_1_b;                                  /*!< BitSize                                                               */
  };
  
  union {
    __I  uint32_t  ARCTANH_2;                       /*!< 0.255412811882995                                                     */
    
    struct {
      __I  uint32_t  ARCTANH_2  : 32;               /*!< 0.255412811882995                                                     */
    } ARCTANH_2_b;                                  /*!< BitSize                                                               */
  };
  
  union {
    __I  uint32_t  ARCTANH_3;                       /*!< 0.125657214140453                                                     */
    
    struct {
      __I  uint32_t  ARCTANH_3  : 32;               /*!< 0.125657214140453                                                     */
    } ARCTANH_3_b;                                  /*!< BitSize                                                               */
  };
  
  union {
    __I  uint32_t  ARCTANH_4;                       /*!< 0.0625815714770030                                                    */
    
    struct {
      __I  uint32_t  ARCTANH_4  : 32;               /*!< 0.0625815714770030                                                    */
    } ARCTANH_4_b;                                  /*!< BitSize                                                               */
  };
  
  union {
    __I  uint32_t  ARCTANH_5;                       /*!< 0.0312601784906670                                                    */
    
    struct {
      __I  uint32_t  ARCTANH_5  : 32;               /*!< 0.0312601784906670                                                    */
    } ARCTANH_5_b;                                  /*!< BitSize                                                               */
  };
  
  union {
    __I  uint32_t  ARCTANH_6;                       /*!< 0.0156262717520522                                                    */
    
    struct {
      __I  uint32_t  ARCTANH_6  : 32;               /*!< 0.0156262717520522                                                    */
    } ARCTANH_6_b;                                  /*!< BitSize                                                               */
  };
  
  union {
    __I  uint32_t  ARCTANH_7;                       /*!< 0.00781265895154042                                                   */
    
    struct {
      __I  uint32_t  ARCTANH_7  : 32;               /*!< 0.00781265895154042                                                   */
    } ARCTANH_7_b;                                  /*!< BitSize                                                               */
  };
  
  union {
    __I  uint32_t  ARCTANH_8;                       /*!< 0.00390626986839683                                                   */
    
    struct {
      __I  uint32_t  ARCTANH_8  : 32;               /*!< 0.00390626986839683                                                   */
    } ARCTANH_8_b;                                  /*!< BitSize                                                               */
  };
  
  union {
    __I  uint32_t  ARCTANH_9;                       /*!< 0.00195312748353255                                                   */
    
    struct {
      __I  uint32_t  ARCTANH_9  : 32;               /*!< 0.00195312748353255                                                   */
    } ARCTANH_9_b;                                  /*!< BitSize                                                               */
  };
  
  union {
    __I  uint32_t  ARCTANH_A;                       /*!< 0.000976562810441036                                                  */
    
    struct {
      __I  uint32_t  ARCTANH_A  : 32;               /*!< 0.000976562810441036                                                  */
    } ARCTANH_A_b;                                  /*!< BitSize                                                               */
  };
  
  union {
    __I  uint32_t  ARCTANH_B;                       /*!< 0.000488281288805113                                                  */
    
    struct {
      __I  uint32_t  ARCTANH_B  : 32;               /*!< 0.000488281288805113                                                  */
    } ARCTANH_B_b;                                  /*!< BitSize                                                               */
  };
  
  union {
    __I  uint32_t  ARCTANH_C;                       /*!< 0.000244140629850639                                                  */
    
    struct {
      __I  uint32_t  ARCTANH_C  : 32;               /*!< 0.000244140629850639                                                  */
    } ARCTANH_C_b;                                  /*!< BitSize                                                               */
  };
  
  union {
    __I  uint32_t  ARCTANH_D;                       /*!< 0.000122070313106330                                                  */
    
    struct {
      __I  uint32_t  ARCTANH_D  : 32;               /*!< 0.000122070313106330                                                  */
    } ARCTANH_D_b;                                  /*!< BitSize                                                               */
  };
  
  union {
    __I  uint32_t  ARCTANH_E;                       /*!< 0.000061035156325791                                                  */
    
    struct {
      __I  uint32_t  ARCTANH_E  : 32;               /*!< 0.000061035156325791                                                  */
    } ARCTANH_E_b;                                  /*!< BitSize                                                               */
  };
  
  union {
    __I  uint32_t  ARCTANH_F;                       /*!< 0.000030517578134473                                                  */
    
    struct {
      __I  uint32_t  ARCTANH_F  : 32;               /*!< 0.000030517578134473                                                  */
    } ARCTANH_F_b;                                  /*!< BitSize                                                               */
  };
  
  union {
    __I  uint32_t  CIRCULAR_K;                      /*!< 0.607252935009250                                                     */
    
    struct {
      __I  uint32_t  CIRCULAR_K : 32;               /*!< 0.607252935009250                                                     */
    } CIRCULAR_K_b;                                 /*!< BitSize                                                               */
  };
  
  union {
    __I  uint32_t  CIRCULAR_K_INV;                  /*!< 1.64676025812007                                                      */
    
    struct {
      __I  uint32_t  CIRCULAR_K_INV: 32;            /*!< 1.64676025812007                                                      */
    } CIRCULAR_K_INV_b;                             /*!< BitSize                                                               */
  };
  
  union {
    __I  uint32_t  HYPERBOLIC_K;                    /*!< 1.717329163039773                                                     */
    
    struct {
      __I  uint32_t  HYPERBOLIC_K: 32;              /*!< 1.717329163039773                                                     */
    } HYPERBOLIC_K_b;                               /*!< BitSize                                                               */
  };
  
  union {
    __I  uint32_t  HYPERBOLIC_K_INV;                /*!< 0.582299550675505                                                     */
    
    struct {
      __I  uint32_t  HYPERBOLIC_K_INV: 32;          /*!< 0.582299550675505                                                     */
    } HYPERBOLIC_K_INV_b;                           /*!< BitSize                                                               */
  };
} OB_CORDIC_Type;


/* ================================================================================ */
/* ================                       MDU                      ================ */
/* ================================================================================ */


/**
  * @brief Multiplication-Division Unit (MDU)
  */

typedef struct {                                    /*!< MDU Structure                                                         */
  
  union {
    __IO uint32_t  ARCON;                           /*!< Arithmetic Control Register.                                          */
    
    struct {
      __IO uint32_t  MDUOP      :  4;               /*!< MDU Operation select enumeration type.                                */
      __IO uint32_t  SIGNE      :  1;               /*!< Sign enable                                                           */
      __IO uint32_t  SLR        :  1;               /*!< Shift direction                                                       */
      __IO uint32_t  MDOV       :  1;               /*!< MDU Overflow flag MDOV                                                */
      __IO uint32_t  MDEF       :  1;               /*!< MDU Error flag MDEF                                                   */
      __IO uint32_t  SC         :  6;               /*!< Shift counter. When set to all '0's, normalize operation is
                                                         selected. After normalization, the 'sc.0'-'sc.4' contain the
                                                          number of normalizing shift performed. When at least one of
                                                          these bit is set high shift operation is selected. The number
                                                          of shifts performed is determined by the number written to 'sc.4'-'sc.0',
                                                          where 'sc.4' is the MSB.                                             */
           uint32_t             :  1;
      __IO uint32_t  TRIG       :  1;               /*!< MDU function trigger.                                                 */
    } ARCON_b;                                      /*!< BitSize                                                               */
  };
  
  union {
    __IO uint32_t  MD0;                             /*!< Multiplication/Division Register.                                     */
    
    struct {
      __IO uint32_t  MD0        : 32;               /*!< Multiplicand, Dividend, Shift or Normalization.                       */
    } MD0_b;                                        /*!< BitSize                                                               */
  };
  
  union {
    __IO uint32_t  MD1;                             /*!< Multiplication/Division Register.                                     */
    
    struct {
      __IO uint32_t  MD1        : 32;               /*!< The product of 32 bit multiplies by 32 bit.                           */
    } MD1_b;                                        /*!< BitSize                                                               */
  };
  
  union {
    __IO uint32_t  MD2;                             /*!< Multiplication/Division Register.                                     */
    
    struct {
      __IO uint32_t  MD2        : 32;               /*!< Multiplier or Divisor.                                                */
    } MD2_b;                                        /*!< BitSize                                                               */
  };
} OB_MDU_Type;


/* --------------------  End of section using anonymous unions  ------------------- */
#if defined(__CC_ARM)
  #pragma pop
#elif defined(__ICCARM__)
  /* leave anonymous unions enabled */
#elif defined(__GNUC__)
  /* anonymous unions are enabled by default */
#elif defined(__TMS470__)
  /* anonymous unions are enabled by default */
#elif defined(__TASKING__)
  #pragma warning restore
#else
  #warning Not supported compiler type
#endif




/* ================================================================================ */
/* ================              Peripheral memory map             ================ */
/* ================================================================================ */

#define OB_I2C0_BASE                    0x40000000UL
#define OB_WWDT_BASE                    0x40004000UL
#define OB_UART0_BASE                   0x40008000UL
#define OB_CT16B0_BASE                  0x4000C000UL
#define OB_PWMTM_BASE                   0x4000F000UL
#define OB_CT16B1_BASE                  0x40010000UL
#define OB_CT32B0_BASE                  0x40014000UL
#define OB_CT32B1_BASE                  0x40018000UL
#define OB_ADC_BASE                     0x4001C000UL
#define OB_I2C1_BASE                    0x40024000UL
#define OB_PWM_BASE                     0x4002C000UL
#define OB_UART1_BASE                   0x40030000UL
#define OB_QEI_BASE                     0x40034000UL
#define OB_FMC_BASE                     0x4003C000UL
#define OB_SPI_BASE                     0x40040000UL
#define OB_ACMP_BASE                    0x40050000UL
#define OB_OPA_BASE                     0x40050080UL
#define OB_RTC_BASE                     0x40058000UL
#define OB_PDMA0_BASE                   0x40100080UL
#define OB_PDMA1_BASE                   0x40100090UL
#define OB_PDMA2_BASE                   0x401000A0UL
#define OB_PDMA3_BASE                   0x401000B0UL
#define OB_CRC_BASE                     0x401000D0UL
#define OB_GPIO0_BASE                   0x50000000UL
#define OB_GPIO1_BASE                   0x50004000UL
#define OB_GPIO2_BASE                   0x50008000UL
#define OB_GPIO3_BASE                   0x5000C000UL
#define OB_SYSCON_BASE                  0x50010000UL
#define OB_CORDIC_BASE                  0x50014000UL
#define OB_MDU_BASE                     0x50014400UL


/* ================================================================================ */
/* ================             Peripheral declaration             ================ */
/* ================================================================================ */

#define OB_I2C0                         ((OB_I2C_Type             *) OB_I2C0_BASE)
#define OB_WWDT                         ((OB_WWDT_Type            *) OB_WWDT_BASE)
#define OB_UART0                        ((OB_UART_Type            *) OB_UART0_BASE)
#define OB_CT16B0                       ((OB_CT16B_Type           *) OB_CT16B0_BASE)
#define OB_PWMTM                        ((OB_PWMTM_Type           *) OB_PWMTM_BASE)
#define OB_CT16B1                       ((OB_CT16B_Type           *) OB_CT16B1_BASE)
#define OB_CT32B0                       ((OB_CT32B_Type           *) OB_CT32B0_BASE)
#define OB_CT32B1                       ((OB_CT32B_Type           *) OB_CT32B1_BASE)
#define OB_ADC                          ((OB_ADC_Type             *) OB_ADC_BASE)
#define OB_I2C1                         ((OB_I2C_Type             *) OB_I2C1_BASE)
#define OB_PWM                          ((OB_PWM_Type             *) OB_PWM_BASE)
#define OB_UART1                        ((OB_UART_Type            *) OB_UART1_BASE)
#define OB_QEI                          ((OB_QEI_Type             *) OB_QEI_BASE)
#define OB_FMC                          ((OB_FMC_Type             *) OB_FMC_BASE)
#define OB_SPI                          ((OB_SPI_Type             *) OB_SPI_BASE)
#define OB_ACMP                         ((OB_ACMP_Type            *) OB_ACMP_BASE)
#define OB_OPA                          ((OB_OPA_Type             *) OB_OPA_BASE)
#define OB_RTC                          ((OB_RTC_Type             *) OB_RTC_BASE)
#define OB_PDMA0                        ((OB_PDMA_Type            *) OB_PDMA0_BASE)
#define OB_PDMA1                        ((OB_PDMA_Type            *) OB_PDMA1_BASE)
#define OB_PDMA2                        ((OB_PDMA_Type            *) OB_PDMA2_BASE)
#define OB_PDMA3                        ((OB_PDMA_Type            *) OB_PDMA3_BASE)
#define OB_CRC                          ((OB_CRC_Type             *) OB_CRC_BASE)
#define OB_GPIO0                        ((OB_GPIO_Type            *) OB_GPIO0_BASE)
#define OB_GPIO1                        ((OB_GPIO_Type            *) OB_GPIO1_BASE)
#define OB_GPIO2                        ((OB_GPIO_Type            *) OB_GPIO2_BASE)
#define OB_GPIO3                        ((OB_GPIO_Type            *) OB_GPIO3_BASE)
#define OB_SYSCON                       ((OB_SYSCON_Type          *) OB_SYSCON_BASE)
#define OB_CORDIC                       ((OB_CORDIC_Type          *) OB_CORDIC_BASE)
#define OB_MDU                          ((OB_MDU_Type             *) OB_MDU_BASE)


/** @} */ /* End of group Device_Peripheral_Registers */
/** @} */ /* End of group OB90A64M1 */
/** @} */ /* End of group On-Bright */

#ifdef __cplusplus
}
#endif


#endif  /* OB90A64M1_H */

