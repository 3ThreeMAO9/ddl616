#include "PCBA_test.h"
#include "hal_uart.h"
#include "hal_gpio.h"
#include "utils.h"
#include "shell.h"
#include "module_spi_flash.h"
#include "module_produce_test.h"
#include "parameter.h"
#include "module_battery.h"
#define OB_LOG_LEVEL OB_LOG_LEVEL_DEBUG
#include "ob_log.h"
#define TAG "PCBA_test"

/***************Variable***************/

// ------------------------------------------

// void pcbaTestSendAckCmd(uint8_t cmd, uint8_t code, uint8_t* data, uint8_t len)
// {
//     uint8_t buffer[PRODUCE_TEST_TX_BUFFER_SIZE];
//     uint8_t sum;

//     buffer[0] = PCBA_TEST_FRAME_HEADER;
//     buffer[1] = (len + 1);
//     buffer[2] = (cmd | PCBA_TEST_ACK_FLAG);
//     buffer[3]  = code;
//     memcpy((&buffer[4]), data, len);

//     sum = (checkSum(buffer, (4 + len)) & 0xFF);

//     buffer[4 + len] = sum;
//     buffer[5 + len] = ((PCBA_TEST_FRAME_TAIL>>8) & 0xFF);
//     buffer[6 + len] = (PCBA_TEST_FRAME_TAIL & 0xFF);

//     //send cmd by uart
//     hal_uartSendBuff(PRODUCE_TEST_UART_GROUP, buffer, (len + 7));
// }

// void pcbaBaseTestAck(uint8_t cmd, uint8_t code)
// {
//     const uint8_t data = 0;
//     if(isTestEnable())
//         pcbaTestSendAckCmd(cmd, code, (uint8_t*)(&data), 0);
// }

// void pcbaCardTestAck(uint8_t code)
// {
//     const uint8_t data = 0;
//     pcbaTestSendAckCmd(PCBA_TEST_CMD_NFC, code, (uint8_t*)(&data), 0);
// }


// void pcbaKeyBoardTestAck(uint8_t code, uint8_t keyValue)
// {
//     pcbaTestSendAckCmd(PCBA_TEST_CMD_KEY_BOARD, code, (uint8_t*)(&keyValue), sizeof(keyValue));
// }

// void pcbaMotorTestAck(uint8_t code, uint16_t current)
// {
//     pcbaTestSendAckCmd(PCBA_TEST_CMD_MOTOR, code, (uint8_t*)(&current), sizeof(current));
// }

// void pcbaKeyTestAck(uint8_t code, uint8_t keyValue)
// {
//     pcbaTestSendAckCmd(PCBA_TEST_CMD_KEY_TEST, code, (uint8_t*)(&keyValue), sizeof(keyValue));
// }

// void pcbaBlueTestAck(uint8_t code, uint16_t hif_tx, uint16_t hif_rx, uint16_t sif_tx, uint16_t sif_rx)
// {
//     uint8_t buf[8] = {0};
//     buf[0] = hif_tx & 0xff;
//     buf[1] = hif_tx >> 8;

//     buf[2] = hif_rx & 0xff;
//     buf[3] = hif_rx >> 8;

//     buf[4] = sif_tx & 0xff;
//     buf[5] = sif_tx >> 8;

//     buf[6] = sif_rx & 0xff;
//     buf[7] = sif_rx >> 8;
//     pcbaTestSendAckCmd(PCBA_TEST_CMD_BLUE_TEST, code, (uint8_t *)(&buf), sizeof(buf));
// }

// void pcbaSelfTestAck(uint8_t code, uint16_t volage)
// {
//     pcba_selftest_t ack;

//     memset((uint8_t*)(&ack), 0, sizeof(pcba_selftest_t));
//     ack.voltage = volage;

//     pcbaTestSendAckCmd(PCBA_TEST_CMD_SELF_TEST, code, (uint8_t*)(&ack), 15);
// }

void pcbaVersionTestAck(uint8_t code)
{
    uint8_t i;
    uint16_t client_project;
    uint16_t outside_version;
    uint16_t voice_version;
    uint8_t buffer[11];

    client_project = readDeviceClientProjectSn();
    outside_version = readDeviceVesion();
    voice_version = readVoiceInfoVersion();

    for (i = 0; i < 3; i++)
    {
        buffer[i] = voice_version % 10;
        voice_version /= 10;
    }

    for (i = 0; i < 3; i++)
    {
        buffer[i + 3] = outside_version % 10;
        outside_version /= 10;
    }

    for (i = 0; i < 5; i++)
    {
        buffer[i + 6] = client_project % 10;
        client_project /= 10;
    }

    OB_LOGI_DUMP(buffer,sizeof(buffer));
    pcbaTestSendAckCmd(PCBA_TEST_CMD_VERSION, code, (uint8_t*)(&buffer), sizeof(buffer));
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC), pcbaVersionTestAck, pcbaVersionTestAck, pcbaVersionTestAck);


// static uint8_t isValidProduceTestPacket(uint8_t* data, uint8_t len)
// {
//     uint16_t tail;
//     uint8_t sum;

// #if (Enabled == PRINTF_TEST)
//     OB_LOGD(TAG,"Uart[%u]-RX: ", PRODUCE_TEST_UART_GROUP);
//     OB_LOGD_DUMP(&data[0],len);
// #endif

//     if (PCBA_TEST_FRAME_HEADER != data[0])
//     {
// #if (Enabled == PRINTF_TEST)
//         OB_LOGE(TAG,"Fail: header[%02X] is err", data[0]);
// #endif
//         return false;
//     }

//     if (len > PRODUCE_TEST_TX_BUFFER_SIZE)
//     {
// #if (Enabled == PRINTF_TEST)
//         OB_LOGE(TAG,"Fail: len[%02X] is out", len);
// #endif
//         return false;
//     }

//     tail = BYTES_TO_UINT16_BE(&data[len - 2]);
//     if (PCBA_TEST_FRAME_TAIL != tail)
//     {
// #if (Enabled == PRINTF_TEST)
//         OB_LOGE(TAG,"Fail: tail[%04X] is err", tail);
// #endif
//         return false;
//     }

//     sum = checkSum(data, (data[1] + 3));
//     if (sum != data[len - 3])
//     {
//         return false;
//     }

//     return true;
// }

void pcbaBatteryCaliAck(uint8_t* data)
{
    // pcba_test_packet_t ack;
    uint16_t uint_voltage;
    int16_t int_voltage;
    // int16_t my_voltage=(int16_t)batteryTaskReadVoltage();
    int16_t my_voltage=(int16_t)TestBatteryVoltage();
    int16_t result;
    uint8_t buffer[4]={0,0,0,0};

    uint_voltage=(data[1]<<8)|data[0];
    int_voltage = (int16_t)uint_voltage;
    result = int_voltage-my_voltage;
    memcpy(&buffer[0],(uint8_t*)&my_voltage,2);
    memcpy(&buffer[2],(uint8_t*)&result,2);
    // uart0_printf("%d - %d = %d\r\n",int_voltage,my_voltage,result);

    pcbaTestSendAckCmd(PCBA_TEST_CMD_BATCALI,!setBatterycali(result,1),buffer,sizeof(buffer));
}

void pcbaReadBatteryCaliAck(uint8_t code)
{
    uint8_t buffer[2]={0,0};

    int16_t my_voltage = ReadBatterycali();
    
    memcpy(&buffer[0],(uint8_t*)&my_voltage,2);
    pcbaTestSendAckCmd(PCBA_TEST_CMD_BATCALI,code,buffer,sizeof(buffer));
}


