/**
 * Copyright(C) 2024 GZ-OB. All Rights Reserved
 * File: produce_test.h
 * Desc: produce test 閰嶇疆
 * Ver : V1.01
 * Revision: Ren
 * Date: 2025-05-14
 *  */

#ifndef PRODUCE_TEST__HH
#define PRODUCE_TEST__HH

#include "config.h"
#include "hal_uart.h"

/*****************Macro****************/
#define PRODUCE_TEST_TX_BUFFER_SIZE         UART0_BUF_LEN

#define PRODUCE_TEST_UART_GROUP             UART_0

#define PRODUCE_TEST_UART_BAUDRATE          UART_BAUDRATE_115200

#define PRODUCE_TEST_ALLLOW_TIME            5000            

//cmd(bit0~6)
#define PRODUCE_TEST_TEST_MODE              0x01
#define PRODUCE_TEST_KEY_BOARD              0x03
#define PRODUCE_TEST_GPIO                   0x06
#define PRODUCE_TEST_SELF_TEST              0x07
#define PRODUCE_TEST_WRITE_SN               0x08
#define PRODUCE_TEST_READ_SN                0x09
#define PRODUCE_TEST_SLEEP                  0x0C
#define PRODUCE_TEST_MOTOR                  0x0E
#define PRODUCE_TEST_VERSION                0x13
#define PRODUCE_TEST_READ_RESULT            0x14

//鍛戒护
#define KDS_CMD_MODE_SEL    0x01
#define KDS_CMD_GET_RESULT  0x0D
#define KDS_CMD_BLE_SN      0x0F
#define KDS_CMD_BATTERY     0x10
#define KDS_CMD_SN          0x16
#define KDS_CMD_MAC         0x19
#define KDS_CMD_SLEEP       0x1A
#define KDS_CMD_BLE_KEY     0x1F
#define KDS_CMD_RESET       0x34
#define KDS_CMD_BLOCKKEY    0x82
#define KDS_CMD_WRITEINFO   0x83
#define KDS_CMD_READINFO    0x84
#define KDS_CMD_VERSION     0x85


//0x01
#define SUB_CMD_MODE_NORMAL 0x00
#define SUB_CMD_MODE_TEST   0x01
#define SUB_CMD_MODE_AGING  0x02
//0x16
#define SUB_CMD_SN_SET  0X11
#define SUB_CMD_SN_READ 0X10
#define SUB_CMD_ESN_READ 0X40
//0x85
#define SUB_VERSION_MAIN    0x06
#define SUB_VERSION_BACK    0x07
#define SUB_VERSION_VOCIE   0x08

//鏁版嵁澶?
#define PACK_HEAD_REQ  0xF5
#define PACK_HEAD_RESP 0x5F
//閿欒鐮?
#define OB_PARA_SUCCES 0x80
#define OB_PARA_ERROR  0x81

//ack(bit7)
#define PRODUCE_TEST_ACK_FLAG               0x80

//header &tail
#define PRODUCE_TEST_FRAME_HEADER           0x5A
#define PRODUCE_TEST_FRAME_TAIL             0x0D0A

#define PCBA_IO_TEST_GROUP_CNT_MAX          (10)
/*****************Enum*****************/
typedef enum{
    PRODUCE_TEST_CODE_SUCCESS = 0x00,
    PRODUCE_TEST_CODE_FAIL,
    PRODUCE_TEST_CODE_SUCCESS_INCEPT,

}produce_test_code_e;

/****************Struct****************/


/***************Variable***************/
#pragma pack(1)
typedef struct{
    uint8_t header;
    uint8_t len;
    uint8_t cmd;
    uint8_t buf[PRODUCE_TEST_TX_BUFFER_SIZE - 3];

}pcba_test_packet_t;

typedef struct{
    uint8_t port;
    uint8_t pin;
    uint8_t level : 4;
    uint8_t mode : 4;

}pcba_io_test_t;

typedef struct{
    uint16_t voltage;
    uint8_t spi_flash;
    uint8_t rtc;
    uint8_t buf[11];

}pcba_selftest_t;

typedef struct 
{
	uint8_t head;
	uint8_t sum[2];
	uint8_t len;
	uint8_t cmd;
	uint8_t buff[PRODUCE_TEST_TX_BUFFER_SIZE - 5];

}para_wr_comm_frame_t;

#pragma pack()

/***************Function***************/
void produceTestUartInit(uint8_t initFlag);
void produceTestSendAckCmd(uint8_t cmd, uint8_t code, uint8_t* data, uint8_t len);

void produceReadVersionAck(uint8_t code);
void produceBaseTestAck(uint8_t cmd, uint8_t code);
void produceMotorTestAck(uint8_t code, uint16_t current);
void produceKeyBoardTestAck(uint8_t code, uint8_t keyValue);
void produceSelfTestResultAck(uint8_t code, uint16_t volage, uint8_t spi_flash, uint8_t rtc);

uint8_t produceTestPacketDecode(pcba_test_packet_t* response_packet);

uint8_t produceGpioTestHandle(uint8_t* data, uint8_t len);

/**************************************/

#endif 
