#include "produce_test.h"
#include "hal_uart.h"
#include "hal_gpio.h"
#include "item_config.h"

#define OB_LOG_LEVEL OB_LOG_LEVEL_DEBUG
#include "ob_log.h"
#define TAG "produce_test"


/***************Variable***************/
uint8_t finger_enable_flag = 1;
// ------------------------------------------
// void produceTestUartInit(uint8_t initFlag)
// {
//     if (initFlag)
//     {
//         hal_uart_Init(PRODUCE_TEST_UART_GROUP, PRODUCE_TEST_UART_BAUDRATE, HAL_GPIO_PULL_NONE);
//     }
//     else
//     {
//         hal_uart_sleep(PRODUCE_TEST_UART_GROUP, 0, false);
//     }
// }

// void produceTestSendAckCmd(uint8_t cmd, uint8_t code, uint8_t *data, uint8_t len)
// {
//     uint8_t buffer[PRODUCE_TEST_TX_BUFFER_SIZE];
//     uint8_t sum;
// #if (Enabled == PRINTF_TEST)
//     uint8_t i;
// #endif
//     buffer[0] = PRODUCE_TEST_FRAME_HEADER;
//     buffer[1] = (len + 1);
//     buffer[2] = (cmd | PRODUCE_TEST_ACK_FLAG);
//     buffer[3] = code;
//     memcpy((&buffer[4]), data, len);

//     sum = (checkSum(buffer, (3 + len)) & 0xFF);

//     buffer[4 + len] = sum;
//     buffer[5 + len] = ((PRODUCE_TEST_FRAME_TAIL >> 8) & 0xFF);
//     buffer[6 + len] = (PRODUCE_TEST_FRAME_TAIL & 0xFF);

//     // send cmd by uart
//     hal_uartSendBuff(PRODUCE_TEST_UART_GROUP, buffer, (len + 7));
// }

// void produceBaseTestAck(uint8_t cmd, uint8_t code)
// {
//     const uint8_t data = 0;
//     produceTestSendAckCmd(cmd, code, (uint8_t *)(&data), 0);
// }

// void produceKeyBoardTestAck(uint8_t code, uint8_t keyValue)
// {
//     produceTestSendAckCmd(PRODUCE_TEST_KEY_BOARD, code, (uint8_t *)(&keyValue), sizeof(keyValue));
// }

// void produceMotorTestAck(uint8_t code, uint16_t current)
// {
//     produceTestSendAckCmd(PRODUCE_TEST_MOTOR, code, (uint8_t *)(&current), sizeof(current));
// }

// void produceSelfTestResultAck(uint8_t code, uint16_t volage, uint8_t spi_flash, uint8_t rtc)
// {
//     pcba_selftest_t ack;

//     memset((uint8_t *)(&ack), 0, sizeof(pcba_selftest_t));
//     ack.voltage = volage;
//     ack.spi_flash = spi_flash;
//     ack.rtc = rtc;

//     produceTestSendAckCmd(PRODUCE_TEST_SELF_TEST, code, (uint8_t *)(&ack), 15);
// }

// void produceReadVersionAck(uint8_t code)
// {
//     uint8_t i;
//     uint16_t client_project;
//     uint16_t outside_version;
//     uint8_t buffer[8];

//     client_project = readDeviceClientProjectSn();
//     outside_version = readDeviceVesion();

//     for (i = 0; i < 3; i++)
//     {
//         buffer[i] = outside_version % 10;
//         outside_version /= 10;
//     }

//     for (i = 0; i < 5; i++)
//     {
//         buffer[i + 3] = client_project % 10;
//         client_project /= 10;
//     }

//     produceTestSendAckCmd(PRODUCE_TEST_VERSION, code, (uint8_t *)(&buffer), sizeof(buffer));
// }

// static uint8_t isValidProduceTestPacket(uint8_t *data, uint8_t len)
// {
//     uint16_t tail;
//     uint8_t sum;

// #if (Enabled == PRINTF_TEST)
//     uint8_t i;
//     OB_LOGW(TAG,"Uart[%u]-RX: ", PRODUCE_TEST_UART_GROUP);
//     OB_LOGW_DUMP(data,len);
// #endif

//     if (PRODUCE_TEST_FRAME_HEADER != data[0])
//     {
// #if (Enabled == PRINTF_TEST)
//         OB_LOGW(TAG,"Fail: header[%02X] is err", data[0]);
// #endif
//         return false;
//     }

//     if (len > PRODUCE_TEST_TX_BUFFER_SIZE)
//     {
// #if (Enabled == PRINTF_TEST)
//         OB_LOGW(TAG,"Fail: len[%02X] is out", len);
// #endif
//         return false;
//     }

//     tail = BYTES_TO_UINT16_BE(&data[len - 2]);
//     if (PRODUCE_TEST_FRAME_TAIL != tail)
//     {
// #if (Enabled == PRINTF_TEST)
//         OB_LOGW(TAG,"Fail: tail[%04X] is err", tail);
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

// static uint8_t isValidKDSTestPacket(uint8_t *data, uint8_t len)
// {
//     uint16_t sum, sum_rec;

//     if (PACK_HEAD_REQ != data[0])
//     {
// #if (Enabled == PRINTF_TEST)
//         OB_LOGW(TAG,"Fail: header[%02X] is err", data[0]);
// #endif
//         return false;
//     }
//     if (len > PRODUCE_TEST_TX_BUFFER_SIZE)
//     {
// #if (Enabled == PRINTF_TEST)
//         OB_LOGW(TAG,"Fail: len[%02X] is out", len);
// #endif
//         return false;
//     }
//     len = data[3];

//     sum = checkSum(&data[4], len);
//     sum_rec = data[2]*256+data[1];

//     if (sum != sum_rec)
//     {
//         // uart0_printf("sum not match\r\n");
//         // uart0_printf("sum: %X %X\r\n", sum, sum_rec);
//         return false;
//     }

//     return true;
// }

// uint8_t produceTestPacketDecode(pcba_test_packet_t *response_packet)
// {
//     uint16_t len;
//     uint8_t *pt = (uint8_t *)(response_packet);

//     len = hal_uart_receive_deal(PRODUCE_TEST_UART_GROUP, pt);
//     if (len)
//     {
//         if (isValidProduceTestPacket(pt, len) || isValidKDSTestPacket(pt, len))
//         {
//             return true;
//         }
//     }

//     return false;
// }


// void hal_gpio_config(HalGpioPort port, HalGpioPin pin, uint8_t mode, uint8_t level)
// {
//     if (port == FINGER_TX_GPIO && pin == FINGER_TX_PIN)
//     {
//         if (finger_enable_flag)
//         {
//             finger_enable_flag = 0;
//             hal_uart_sleep(FINGER_UART_GROUP, 0, false);
//             HAL_GPIO_DisableIRQ(FINGER_INT_GPIO, FINGER_INT_PIN);
//             CLEAR_FINGER_INT_STATE();
//         }
//     }
//     else if (port == FINGER_RX_GPIO && pin == FINGER_RX_PIN)
//     {
//         if (finger_enable_flag)
//         {
//             finger_enable_flag = 0;
//             hal_uart_sleep(FINGER_UART_GROUP, 0, false);
//             HAL_GPIO_DisableIRQ(FINGER_INT_GPIO, FINGER_INT_PIN);
//             CLEAR_FINGER_INT_STATE();
//         }
//     }
//     else if (port == FINGER_INT_GPIO && pin == FINGER_INT_PIN)
//     {
//         if (finger_enable_flag)
//         {
//             finger_enable_flag = 0;
//             hal_uart_sleep(FINGER_UART_GROUP, 0, false);
//             HAL_GPIO_DisableIRQ(FINGER_INT_GPIO, FINGER_INT_PIN);
//             CLEAR_FINGER_INT_STATE();
//         }
//     }
//     if (mode == 0x01) // output
//     {
//         HAL_GPIO_Init(port, pin, HAL_GPIO_MODE_OUTPUT_PP, HAL_GPIO_PULL_NONE);
//         if ((level & 0xF) == 0x01)
//         {
//             HAL_GPIO_Write(port, pin, 1);
//         }
//         else if ((level & 0xF) == 0x00)
//         {
//             HAL_GPIO_Write(port, pin, 0);
//         }
//     }
//     else if (mode == 0x00) // input
//     {
//         HAL_GPIO_Init(port, pin, HAL_GPIO_MODE_INPUT, HAL_GPIO_PULL_NONE);
//     }
//     else if (mode == 0x02) // input
//     {
//         HAL_GPIO_Init(port, pin, HAL_GPIO_MODE_INPUT, HAL_GPIO_PULL_HIGH);
//     }
//     else if (mode == 0x03)
//     {
//         HAL_GPIO_Init(port, pin, HAL_GPIO_MODE_INPUT, HAL_GPIO_PULL_LOW);
//     }
// }

// uint8_t produceGpioTestHandle(uint8_t *data, uint8_t len)
// {
//     uint8_t i;
//     uint8_t cnt;
//     uint8_t mode_tab[2] = {HAL_GPIO_MODE_INPUT, HAL_GPIO_MODE_OUTPUT_PP};
//     HalGpioPort port_tab[HAL_GPIO_PORT_INVALID] = {HAL_GPIO_PORT0, HAL_GPIO_PORT1, HAL_GPIO_PORT2, HAL_GPIO_PORT3};
//     HalGpioPin pin_tab[HAL_GPIO_PIN_INVALID] = {HAL_GPIO_PIN0, HAL_GPIO_PIN1, HAL_GPIO_PIN2, HAL_GPIO_PIN3,
//                                               HAL_GPIO_PIN4, HAL_GPIO_PIN5, HAL_GPIO_PIN6, HAL_GPIO_PIN7,
//                                               HAL_GPIO_PIN8, HAL_GPIO_PIN9, HAL_GPIO_PIN10, HAL_GPIO_PIN11,
//                                               HAL_GPIO_PIN12, HAL_GPIO_PIN13, HAL_GPIO_PIN14, HAL_GPIO_PIN15};

//     pcba_io_test_t *gpioConfig = (pcba_io_test_t *)(data);

//     if (len % sizeof(pcba_io_test_t))
//     {
// #if (Enabled == PRINTF_TEST)
//         OB_LOGW(TAG,"Err: len[%u] is't multiple of [%u]", len, sizeof(pcba_io_test_t));
// #endif
//         return false;
//     }
//     cnt = len / sizeof(pcba_io_test_t);
//     if (cnt >= PCBA_IO_TEST_GROUP_CNT_MAX)
//     {
// #if (Enabled == PRINTF_TEST)
//         OB_LOGW(TAG,"Err: io group cnt[%u] is out of [%u]", cnt, PCBA_IO_TEST_GROUP_CNT_MAX);
// #endif
//         return false;
//     }

//     for (i = 0; i < cnt; i++)
//     {
// #if (Enabled == PRINTF_TEST)
//         OB_LOGW(TAG,"group[%u]: port-%u, pin-%u, mode-%u, level-%u", i, gpioConfig[i].port, gpioConfig[i].pin, gpioConfig[i].mode, gpioConfig[i].level);
// #endif
//         hal_gpio_config(port_tab[gpioConfig[i].port], pin_tab[gpioConfig[i].pin], mode_tab[gpioConfig[i].mode], gpioConfig[i].level);
//     }

//     return true;
// }
