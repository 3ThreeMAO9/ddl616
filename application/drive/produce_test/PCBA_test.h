/**
 * Copyright(C) 2024 GZ-OB. All Rights Reserved
 * File: PCBA_test.h
 * Desc: PCBA浜ф祴
 * Ver : V1.00
 * Revision: Ren
 * Date: 2025-01-10
 *  */

#ifndef PCBA_TEST__HH
#define PCBA_TEST__HH

#include "config.h"
#include "produce_test.h"

/*****************Macro****************/
//cmd(bit0~6)
#define PCBA_TEST_CMD_TEST_MODE             0x01
#define PCBA_TEST_CMD_NFC                   0x02
#define PCBA_TEST_CMD_KEY_BOARD             0x03
#define PCBA_TEST_CMD_VOICE                 0x04
#define PCBA_TEST_CMD_GPIO                  0x06
#define PCBA_TEST_CMD_SELF_TEST             0x07
#define PCBA_TEST_CMD_WRITE_SN              0x08
#define PCBA_TEST_CMD_READ_SN               0x09
#define PCBA_TEST_CMD_SLEEP                 0x0C
#define PCBA_TEST_CMD_MOTOR                 0x0E
#define PCBA_TEST_CMD_VERSION               0x13
#define PCBA_TEST_CMD_BATCALI               0x14
#define PCBA_TEST_CMD_READ_BATCALI          0x15
#define PCBA_TEST_CMD_BLUE_TEST             0x21
#define PCBA_TEST_CMD_KEY_TEST              0x22


//ack(bit7)
#define PCBA_TEST_ACK_FLAG                  0x80

//header &tail
#define PCBA_TEST_FRAME_HEADER              0x5A
#define PCBA_TEST_FRAME_TAIL                0x0D0A

#define PCBA_IO_TEST_GROUP_CNT_MAX          (10)

/*****************Enum*****************/
typedef enum{
    PCBA_TEST_CODE_SUCCESS = 0x00,
    PCBA_TEST_CODE_FAIL,
    PCBA_TEST_CODE_SUCCESS_INCEPT,

}PCBA_test_code_t;

typedef enum{
    PCBA_TEST_GPIO_MODE_INPUT = 0,
    PCBA_TEST_GPIO_MODE_PUSH_PULL,

    PCBA_TEST_GPIO_MODE_CNT,

}PCBA_test_gpio_mode_t;


/***************Variable***************/


/***************Function***************/
void pcbaTestSendAckCmd(uint8_t cmd, uint8_t code, uint8_t* data, uint8_t len);

void pcbaVersionTestAck(uint8_t code);
void pcbaBaseTestAck(uint8_t cmd, uint8_t code);
void pcbaMotorTestAck(uint8_t code, uint16_t current);
void pcbaCardTestAck(uint8_t code);
void pcbaKeyBoardTestAck(uint8_t code, uint8_t keyValue);
void pcbaSelfTestAck(uint8_t code, uint16_t voltage);
void pcbaBlueTestAck(uint8_t code, uint16_t hif_tx, uint16_t hif_rx, uint16_t sif_tx, uint16_t sif_rx);
void pcbaBatteryCaliAck(uint8_t* data);
void pcbaReadBatteryCaliAck(uint8_t code);
void pcbaKeyTestAck(uint8_t code, uint8_t keyValue);
/**************************************/

#endif 
