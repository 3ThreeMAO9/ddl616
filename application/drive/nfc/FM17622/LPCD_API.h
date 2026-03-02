#ifndef _LPCD_API_H
#define _LPCD_API_H

#include "config.h"


/*****************Macro****************/


/*****************Enum*****************/
typedef enum {
    LPCD_MODE_ENABLE = 0,
    LPCD_MODE_DISABLE,

}lpcd_mode_e;

/****************Struct****************/
typedef struct {
    uint8_t state: 3;               // NFC工作模式
    uint8_t encrypt : 1;            // 1， 加密； 0， 解密
    uint8_t encrypt_type : 2;       // 加密类型，nfc_encrypt_mode_e

    uint8_t block_sel : 4;          // bit = 1， 读取对应块； bit = 0， 不读取对应块
    uint8_t sector_id : 6;          // 扇区ID （0~63）

}nfc_attribute_t;

/**************************************/
typedef unsigned char (*card_handle_callback_t)(const nfc_attribute_t*);


/***************Variable***************/


/***************Function***************/
unsigned char Lpcd_Init_Register(unsigned char period, unsigned char thresh);
unsigned int Lpcd_Get_ADC_Value(void);
unsigned char Lpcd_IRQ_Event(const nfc_attribute_t* nfc_attr, card_handle_callback_t callback);
uint8_t is_Lpcd_wake(void);

/**************************************/

#endif
