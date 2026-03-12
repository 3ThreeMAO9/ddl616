/**
 * Copyright (c) 2025 GZ-OB, All rights reserved.
 * File name: item_22222.h
 * Desc:
 * Version: 1.0.0
 * Revision: James_Zhang
 * Date: 2025-12-01
 */

#ifndef _ITEM_22222_H_
#define _ITEM_22222_H_
#if (CLIENT_ITEM_SN_22222 == CLIENT_ITEM_SN)
#include "board_RLFA166M1_V1.h"

/***********Macro***********/
//firmware version
#define MINOR_VERSION    PROJECT_NUMBER
#define PATCH_VERSION    1


/***********Enum***********/
#define TOUCH_IC_CH0    0x20
#define TOUCH_IC_CH1    0x20
#define TOUCH_IC_CH2    0x20
#define TOUCH_IC_CH3    0x20
#define TOUCH_IC_CH4    0x20
#define TOUCH_IC_CH5    0x20
#define TOUCH_IC_CH6    0x20
#define TOUCH_IC_CH7    0x20
#define TOUCH_IC_CH8    0x20
#define TOUCH_IC_CH9    0x20
#define TOUCH_IC_CH10   0x20
#define TOUCH_IC_CH11   0x20
#define TOUCH_IC_CH12   0x20
#define TOUCH_IC_CH13   0x20
#define TOUCH_IC_CH14   0x20
#define TOUCH_IC_CH15   0x20

#define NFC_CHECK_ADC                   0x01
#define NFC_CHECK_LPCD                  0x02
#define NFC_CHECK_SEL                   NFC_CHECK_LPCD        // NFC的检测方式

#define NFC_WAKE_UP_READ_CARD           Disabled       // NFC唤醒定时读卡

#ifndef NFC_SACN_INTERVAL
#define NFC_SACN_INTERVAL               (1900)        // ms
#endif

#if(NFC_CHECK_SEL == NFC_CHECK_ADC)
#define NFC_READ_CARD_POWER             (0x20)        //This is nfc read the card power
#define NFC_CHECK_CARD_POWER            (0x08)        //This is nfc sleep check the card power
#define NFC_CHECK_CARD_THRESHOLD_VALUE  (10)          //This is nfc sleep check the card threshold value

#elif(NFC_CHECK_SEL == NFC_CHECK_LPCD)
#define NFC_READ_CARD_POWER             (0x10)        //This is nfc read the card power
#define NFC_CHECK_CARD_POWER            (0x08)        //This is nfc sleep check the card power
#define NFC_CHECK_CARD_THRESHOLD_VALUE  (10)          //This is nfc sleep check the card threshold value

#define LPCD_THRESHOLD                  (80)          // LPCD参考ADC值
#define LPCD_THRESHOLD_MAX              (90)          // LPCD检测阈值设置
#define LPCD_CWP                        (50)          // LPCD P管功率 0~63
#define LPCD_CWN                        (13)          // LPCD N管功率 0~15
#define LPCD_SLEEPTIME                  (16)          // LPCD 唤醒间隔时间，每一档为32ms，休眠时间：（16+1）*32=544ms

#define DELAT                           (10)          //灵敏度 1-15  读取ADC值与设定的参考ADC值，ADC变化值
#define SWINGSCNT                       (7)           //+1+2us,探卡时，发射波形时长

#endif

#define RADAR_GAIN                      (7)
#define RADAR_HIGH_VALUE                (20)
#define RADAR_MIDDLE_VALUE              (30)
#define RADAR_LOW_VALUE                 (40)

#define RADAR_CHECK_IO                  (0x01)
#define RADAR_CHECK_IIC                 (0x02)
#define RADAR_CHECK_SEL                 RADAR_CHECK_IO

/*******************  wake source  *******************/
#define WAKE_SOURCE_NULL                            0x0000
#define WAKE_SOURCE_KEY_BOARD                       0x0001
#define WAKE_SOURCE_FINGER                          0x0002
#define WAKE_SOURCE_NFC                             0x0004
#define WAKE_SOURCE_TAMPER_KEY                      0x0008
#define WAKE_SOURCE_RADAR                           0x0040

/********************  time out  *********************/
#define WORK_TIME_OUT_VAULE                         (10 * 1000)       // ms
#define WAKE_TIME_OUT_VAULE                         (2 * 1000)       // ms

/*****************************************************/
/***********Function***********/


/*****************************/


#endif /* CLIENT_ITEM_SN_22222 == CLIENT_ITEM_SN */
#endif /* _ITEM_22222_H_ */
