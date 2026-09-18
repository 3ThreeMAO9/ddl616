#ifndef _NFC_API_H
#define _NFC_API_H

#include "config.h"


/*****************Macro****************/
#define NFC_SCAN_TIME_OUT           1000
#define NFC_READ_TIME_OUT           1900
/*****************Enum*****************/
typedef enum {
    NFC_MODE_NONE = 0,          // NONE
    NFC_MODE_DEEP_SLEEP,        // 关闭LPCD
    NFC_MODE_CHECK,             // LPCD or adc检卡
    NFC_MODE_IDLE,              // 关闭天线状态
    NFC_MODE_SCAN,              // 定时扫描
    NFC_MODE_FUNC,              // 功能卡

    NFC_MODE_READ_BLOCK,        // 读块信息
    NFC_MODE_WRITE_BLOCK,       // 写块信息
    NFC_MODE_CARRIER,           // 载波测试(天线打开)
    
} nfc_mode_e;

typedef enum {
    NFC_TYPE_MIFARE_1K = 0,     // 1K的M1卡 (ATQA = 0x0004/0x0400; SAK = 0x08/0x88)
    NFC_TYPE_MIFARE_4K,         // 4K的M1卡 (ATQA = 0x0002; SAK = 0x18)
    NFC_TYPE_MIFARE_PLUS,       // 升级版的M1卡 (ATQA = 0x0001; SAK = 0x18)
    NFC_TYPE_DESFIRE,           // CPU卡 (ATQA = 0x0003; SAK = 0x20)
    NFC_TYPE_NTAG21X,           // NFC Forum Type2标签 (ATQA = 0x0004; SAK = 0x00)
    NFC_TYPE_EM4100_ID,         // 低频卡 （只能读ID）

} nfc_type_e;

typedef enum {
    NFC_ENCRYPT_NONE = 0,
    NFC_ENCRYPT_OB_DEFAULT,     // OB默认加密方式

}nfc_encrypt_mode_e;

/****************Struct****************/
typedef struct {
    uint8_t state: 3;               // NFC工作模式
    uint8_t encrypt : 1;            // 1， 加密； 0， 解密
    uint8_t encrypt_type : 2;       // 加密类型，nfc_encrypt_mode_e

    uint8_t block_sel : 4;          // bit = 1， 读取对应块； bit = 0， 不读取对应块
    uint8_t sector_id : 6;          // 扇区ID （0~63）

}nfc_attribute_t;

typedef struct {
    uint8_t init : 1;
    uint8_t release : 1;
    uint8_t mode : 4;
    uint8_t wake : 1;

    uint32_t time_out;

    nfc_attribute_t attr;

    uint8_t block_buffer[3][16];
    
} nfc_handle_t;

typedef struct {
    uint8_t state;              // 状态
    uint8_t card_id[8];         // 卡UID
    uint8_t size : 4;           // 卡UID长度
    uint8_t type: 3;            // 卡类型（nfc_type_e）
    uint8_t valid: 1;           // 卡片是否有效标志（如需限制部分类型卡片操作提示失败，可通过该字段返回结果）

    uint8_t sector_valid : 1;   // 扇区数据有效标识（用于传递扇区数据）
    uint8_t sector_id : 7;      // 扇区ID（0~127）
    uint16_t para;
    uint8_t *sector_pt;         // 指向扇区数据地址

}nfc_event_t;

typedef struct{
    uint32_t gay_value;
    uint32_t ave_value;
    uint32_t adc_voltage;
    uint32_t cal_cnt;
    uint32_t detection_count;
}nfc_adc_t;


/**************************************/
typedef unsigned char (*card_handle_callback_t)(const nfc_attribute_t*);
typedef void (*nfc_event_callback_t)(nfc_event_t*);

/***************Variable***************/


/***************Function***************/
uint8_t nfc_init(const nfc_attribute_t* nfc_attr);
uint8_t nfc_set_attr(const nfc_attribute_t* nfc_attr);
uint8_t nfc_set_mode(uint8_t mode);
void nfc_loop(void);
void nfc_event_register_callback(nfc_event_callback_t callback);
uint8_t is_nfc_wake(void);
void nfc_sacn_handle_get_tick(uint32_t time);

/**************************************/

#endif
