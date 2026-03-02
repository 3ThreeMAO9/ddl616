#ifndef PROTOCOL_FUNC_CARD__HH
#define PROTOCOL_FUNC_CARD__HH

#include "config.h"

/*****************Enum*****************/
typedef enum {
    FUNC_CARD_CMD_NONE = 0,

    FUNC_CARD_CMD_VERSION,      // 版本卡
    FUNC_CARD_CMD_SENSITY,      // 灵敏度卡
    FUNC_CARD_CMD_AGING,        // 老化卡
    FUNC_CARD_CMD_2_4G,         // 2.4G载波卡
    FUNC_CARD_CMD_FT,           // 整机测试卡

    FUNC_CARD_CMD_CNT,

}func_card_cmd_e;

typedef enum {
    FUNC_CARD_VERSION_MASTER = 1,
    FUNC_CARD_VERSION_CHILD,
    FUNC_CARD_VERSION_VOICE,

    FUNC_CARD_VERSION_CNT,

}func_card_version_type_e;

/*****************Macro****************/
#define FUNC_CARD_HEADER                0xAABB

#define FUNC_CARD_BUFFER_SIZE           (41)

/****************Struct****************/
typedef struct {
    uint16_t header;
    uint16_t sum;
    uint16_t cmd;
    uint8_t lenth;
    uint8_t buffer[FUNC_CARD_BUFFER_SIZE];

}func_card_pkt_t;

typedef struct {
    uint16_t value;
    uint8_t error_range;

}func_card_param_t;


/***************Variable***************/


/***************Function***************/
uint8_t read_func_card_pkt_cmd(const func_card_pkt_t* packet);

/**************************************/

#endif 
