;/**************************************************************************//**
; * @file     startup_OB90A64M1.s
; * @brief    CMSIS Cortex-M0 Core Device Startup File
; *           for the OB90A64M1 Device Series
; * @version  0.1
; * @date     2018/05/10
; *------- <<< Use Configuration Wizard in Context Menu >>> ------------------
; *
; * @note
; * Copyright (C) 2005-2018 On-Bright Electronics Co., Ltd. All rights reserved.
; *
; * @par
; * ARM Limited (ARM) is supplying this software for use with Cortex-M 
; * processor based microcontrollers.  This file can be freely distributed 
; * within development tools that are supporting such ARM based processors. 
; *
; * @par
; * THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
; * OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
; * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
; * ARM SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR
; * CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
; *
; ******************************************************************************/

                PRESERVE8
                THUMB

; Vector Table Mapped to Address 0 at Reset
                AREA    RESET, DATA, READONLY

                IMPORT  |Image$$ARM_LIB_STACK$$ZI$$Limit|

__Vectors       DCD     |Image$$ARM_LIB_STACK$$ZI$$Limit|  ; Top of Stack
                DCD     Reset_Handler               ; Reset Handler
                DCD     NMI_Handler                 ; NMI Handler
                DCD     HardFault_Handler           ; Hard Fault Handler
                DCD     0                           ; Reserved
                DCD     0                           ; Reserved
                DCD     0                           ; Reserved
                DCD     0                           ; Reserved
                DCD     0                           ; Reserved
                DCD     0                           ; Reserved
                DCD     0                           ; Reserved
                DCD     SVC_Handler                 ; SVCall Handler
                DCD     0                           ; Reserved
                DCD     0                           ; Reserved
                DCD     PendSV_Handler              ; PendSV Handler
                DCD     SysTick_Handler             ; SysTick Handler

                ; External Interrupts
                DCD     0                           ; 16+ 0: Reserved
                DCD     I2C1_IRQHandler             ; 16+ 1: I2C1
                DCD     LVI_IRQHandler              ; 16+ 2: LVI
                DCD     ACMP_IRQHandler             ; 16+ 3: ACMP
                DCD     RTC_IRQHandler              ; 16+ 4: RTC
                DCD     I2C0_IRQHandler             ; 16+ 5: I2C0
                DCD     CT16B0_IRQHandler           ; 16+ 6: CT16B0
                DCD     CT16B1_IRQHandler           ; 16+ 7: CT16B1
                DCD     CT32B0_IRQHandler           ; 16+ 8: CT32B0
                DCD     CT32B1_IRQHandler           ; 16+ 9: CT32B1
                DCD     SPI_IRQHandler              ; 16+10: SPI
                DCD     UART0_IRQHandler            ; 16+11: UART0
                DCD     QEI_IRQHandler              ; 16+12: QEI
                DCD     PDMA_IRQHandler             ; 16+13: PDMA
                DCD     PWM_IRQHandler              ; 16+14: PWM
                DCD     WDT_IRQHandler              ; 16+15: WDT
                DCD     ADC_IRQHandler              ; 16+16: ADC
                DCD     UART1_IRQHandler            ; 16+17: UART1
                DCD     GPIO0_IRQHandler            ; 16+18: GPIO0
                DCD     GPIO1_IRQHandler            ; 16+19: GPIO1
                DCD     GPIO2_IRQHandler            ; 16+20: GPIO2
                DCD     GPIO3_IRQHandler            ; 16+21: GPIO3
                DCD     PWMTM_IRQHandler            ; 16+22: PWMTM
                DCD     0                           ; 16+23: Reserved
                DCD     0                           ; 16+24: Reserved
                DCD     0                           ; 16+25: Reserved
                DCD     0                           ; 16+26: Reserved
                DCD     0                           ; 16+27: Reserved
                DCD     0                           ; 16+28: Reserved
                DCD     0                           ; 16+29: Reserved
                DCD     0                           ; 16+30: Reserved
                DCD     0                           ; 16+31: Reserved

__Vectors_End

__Vectors_Size  EQU  __Vectors_End - __Vectors

                AREA    |.text|, CODE, READONLY


; Reset Handler

Reset_Handler   PROC
                IMPORT  SystemInit
                IMPORT  boot_main
                
                LDR     R0, =SystemInit
                BLX     R0
                LDR     R0, =boot_main
                BX      R0
                ENDP


; Go to app
App_Go          PROC
                IMPORT  |Image$$ARM_LIB_STACK$$ZI$$Limit|
                IMPORT  g_app_start_addr
                EXPORT  App_Go

                LDR     R0, =|Image$$ARM_LIB_STACK$$ZI$$Limit|
                MSR     MSP, R0
                LDR     R0, =g_app_start_addr
                LDR     R0, [R0]
                BX      R0
                ENDP


IRQ_Handler     PROC
                IMPORT  irq_switch_handler

                PUSH    {LR}
                BL.W    irq_switch_handler
                POP     {PC}
                ENDP


                MACRO
                Set_Irq_Handler $Handler_Name, $index
$Handler_Name   PROC
                MOVS    R0, #$index
                B       IRQ_Handler
                ENDP
                MEND


; Set IRQ Handler for bootloader
                Set_Irq_Handler NMI_Handler, 2
                Set_Irq_Handler HardFault_Handler, 3
                Set_Irq_Handler SVC_Handler, 11
                Set_Irq_Handler PendSV_Handler, 14
                Set_Irq_Handler SysTick_Handler, 15

                Set_Irq_Handler I2C1_IRQHandler, 17
                Set_Irq_Handler LVI_IRQHandler, 18
                Set_Irq_Handler ACMP_IRQHandler, 19
                Set_Irq_Handler RTC_IRQHandler, 20
                Set_Irq_Handler I2C0_IRQHandler, 21
                Set_Irq_Handler CT16B0_IRQHandler, 22
                Set_Irq_Handler CT16B1_IRQHandler, 23
                Set_Irq_Handler CT32B0_IRQHandler, 24
                Set_Irq_Handler CT32B1_IRQHandler, 25
                Set_Irq_Handler SPI_IRQHandler, 26
                Set_Irq_Handler UART0_IRQHandler, 27
                Set_Irq_Handler QEI_IRQHandler, 28
                Set_Irq_Handler PDMA_IRQHandler, 29
                Set_Irq_Handler PWM_IRQHandler, 30
                Set_Irq_Handler WDT_IRQHandler, 31
                Set_Irq_Handler ADC_IRQHandler, 32
                Set_Irq_Handler UART1_IRQHandler, 33
                Set_Irq_Handler GPIO0_IRQHandler, 34
                Set_Irq_Handler GPIO1_IRQHandler, 35
                Set_Irq_Handler GPIO2_IRQHandler, 36
                Set_Irq_Handler GPIO3_IRQHandler, 37
                Set_Irq_Handler PWMTM_IRQHandler, 38

                ALIGN


                END
