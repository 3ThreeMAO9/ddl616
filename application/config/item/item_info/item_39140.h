/**
 * Copyright (c) 2025 GZ-OB, All rights reserved.
 * File name: item_39140.h
 * Desc:
 * Version: 1.0.0
 * Revision: James_Zhang
 * Date: 2025-12-01
 */

#ifndef _ITEM_39140_H_
#define _ITEM_39140_H_

#include "board_RLSA120M_V1.h"

/***********Macro***********/
//firmware version
#define MINOR_VERSION               PROJECT_NUMBER
#define PATCH_VERSION               2


#define PRINTF_FUN                  Enabled

#define PRINTF_USER                 PRINTF_FUN
#define PRINTF_FLASH                Disabled
#define PRINTF_ERR                  PRINTF_FUN
#define PRINTF_PASSWORD             Disabled
#define PRINTF_MOTOR                Disabled
#define PRINTF_FSM                  PRINTF_FUN
#define PRINTF_HMI                  PRINTF_FUN
#define PRINTF_VOLTAGE              Disabled
#define PRINTF_TEST                 PRINTF_FUN

/***********Enum***********/

#define TOUCH_IC_CH0    0x25    //
#define TOUCH_IC_CH1    0x25    //
#define TOUCH_IC_CH2    0x25    //
#define TOUCH_IC_CH3    0x25    //
#define TOUCH_IC_CH4    0x25    //
#define TOUCH_IC_CH5    0x25    //
#define TOUCH_IC_CH6    0x25    //
#define TOUCH_IC_CH7    0x25    //
#define TOUCH_IC_CH8    0x25    //
#define TOUCH_IC_CH9    0x25    //
#define TOUCH_IC_CH10   0x25    //
#define TOUCH_IC_CH11   0x25    //
#define TOUCH_IC_CH12   0x25    //
#define TOUCH_IC_CH13   0x3F    //无对应按键
#define TOUCH_IC_CH14   0x3F    //无对应按键
#define TOUCH_IC_CH15   0x3F    //无对应按键


#define NFC_CHECK_ADC                   0x01
#define NFC_CHECK_LPCD                  0x02
#define NFC_CHECK_SEL                   NFC_CHECK_ADC        // NFC的检测方式

#define NFC_WAKE_UP_READ_CARD           Disabled       // NFC唤醒定时读卡

#ifndef NFC_SACN_INTERVAL
#define NFC_SACN_INTERVAL               (1900)        // ms
#endif

#define OB_LOG_UART_SIMU                0   // 模拟UART（非软串口LOG_TX）
#define OB_LOG_UART_1                   1   // 硬件串口LOG_TX1

#define OB_LOG_UART_SEL                 OB_LOG_UART_1


#if(NFC_CHECK_SEL == NFC_CHECK_ADC)
#define NFC_READ_CARD_POWER             (0x20)        //This is nfc read the card power
#define NFC_CHECK_CARD_POWER            (0x08)        //This is nfc sleep check the card power
#define NFC_CHECK_CARD_THRESHOLD_VALUE  (10)          //This is nfc sleep check the card threshold value

#elif(NFC_CHECK_SEL == NFC_CHECK_LPCD)
#define NFC_READ_CARD_POWER             (0x10)        //This is nfc read the card power
#define NFC_CHECK_CARD_POWER            (0x08)        //This is nfc sleep check the card power
#define NFC_CHECK_CARD_THRESHOLD_VALUE  (10)          //This is nfc sleep check the card threshold value

#define LPCD_THRESHOLD                  (70)          // LPCD参考ADC值
// #define LPCD_THRESHOLD_MAX              (90)          // LPCD检测阈值设置
#define LPCD_CWP                        (50)          // LPCD P管功率 0~63
#define LPCD_CWN                        (13)          // LPCD N管功率 0~15
#define LPCD_SLEEPTIME                  (16)          // LPCD 唤醒间隔时间，每一档为32ms，休眠时间：（16+1）*32=544ms

// #define DELAT                           (10)          //灵敏度 1-15  读取ADC值与设定的参考ADC值，ADC变化值
// #define SWINGSCNT                       (7)           //+1+2us,探卡时，发射波形时长

#endif

#define RADAR_GAIN                      (7)
#define RADAR_HIGH_VALUE                (20)
#define RADAR_MIDDLE_VALUE              (30)
#define RADAR_LOW_VALUE                 (40)

#define RADAR_CHECK_IO                  (0x01)
#define RADAR_CHECK_IIC                 (0x02)
#define RADAR_CHECK_SEL                 RADAR_CHECK_IO

/***********Function***********/


/*****************************/

#endif /* _ITEM_39140_H_ */
