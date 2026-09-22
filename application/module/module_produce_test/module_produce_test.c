#include "module_produce_test.h"
#include "PCBA_test.h"
#include "module_hmi.h"
#include "system_timer.h"
#include "parameter.h"
#include "event.h"
#include "user.h"
#include "task_battery.h"
#include "hal_rtc.h"
#include "produce_test.h"
#include "hal_gpio.h"
#include "config.h"
#include "hal_wdt.h"

#define OB_LOG_LEVEL OB_LOG_LEVEL_DEBUG
#include "ob_log.h"
#define TAG "module_produce_test"

/***************Variable***************/
static produce_test_handle_t produceTestHandle;
static produce_test_callback_t produceTest_callback;
// static uint8_t test_enable = 0;
// static uint32_t test_allow_time = 0;
// static uint8_t testing = 0;
// ------------------------------------------
static void produceTestHandleEvent_callback(uint8_t result, uint8_t value)
{
    if (NULL != produceTest_callback)
    {
        produceTest_callback(result, value);
    }
#if (Enabled == PRINTF_ERR)
    else
    {
        OB_LOGE(TAG, "Err: produce test' callback is null");
    }
#endif
}

//  @brief 娉ㄥ唽鍥炶皟鍑芥暟
//  @param callback
void produceTestEventRegister_callback(produce_test_callback_t callback)
{
    produceTest_callback = callback;

#if (Enabled == PRINTF_TEST)
    OB_LOGD(TAG, "produce_test_register_callback:%X", callback);
#endif
}

// void produceTestHwConfig(uint8_t initFlag)
// {
//     if (isEmptyUser(false))
//     {
//         produceTestUartInit(initFlag);
//         test_enable = initFlag;
//         test_allow_time = system_inc_time_cnt(PRODUCE_TEST_ALLLOW_TIME);
//     }
//     else
//     {
//         produceTestUartInit(0);
//         test_enable = 0;
//     }
// }

void produceTestInit(void)
{
    const uint8_t keyBoardTab[KEY_CNT] = DEVICE_TEST_KEY_BOARD_TAB;
    memset((uint8_t *)(&produceTestHandle), 0, sizeof(produce_test_handle_t));
    memcpy((uint8_t *)(produceTestHandle.keyBoard.tab), keyBoardTab, KEY_CNT);
    produceInfoInit();
    // setProduceReboot(0);
    // if (isEmptyUser(false))
    //     dev_init_resp(); // 没用户，默认上电会发送数据给产测工具
    produceTestHandleEvent_callback(EVENT_RESULT_PRODUCE_INIT, 0);
}

// uint8_t pcbaKeyBoardTestPressHandle(uint8_t keyValue)
// {
// #if (Enabled == PRINTF_TEST)
//     OB_LOGD(TAG, "key board test: [%u, %u]", keyValue, produceTestHandle.keyBoard.tab[produceTestHandle.keyBoard.step]);
// #endif
//     if (produceTestHandle.keyBoard.tab[produceTestHandle.keyBoard.step] == keyValue)
//     {
//         produceTestHandle.keyBoard.step++;
//         if (produceTestHandle.keyBoard.step >= KEY_CNT)
//         {
//             pcbaKeyBoardTestAck(PCBA_TEST_CODE_SUCCESS, keyValue);
//             return true;
//         }
//         else if (true == produceTestHandle.keyBoard.pressReport)
//         {
//             pcbaKeyBoardTestAck(PCBA_TEST_CODE_SUCCESS_INCEPT, keyValue);
//         }

//         return 0xFF;
//     }
    
//     return false;
// }


// void pcbaNfcTestPressHandle(uint8_t flag)
// {
//     if(true == flag)
//         pcbaCardTestAck(PCBA_TEST_CODE_SUCCESS);
//     else
//         pcbaCardTestAck(PCBA_TEST_CODE_FAIL);
// }

// void pcbaMotorTestHandle(uint32_t value)
// {
//     pcbaMotorTestAck(PCBA_TEST_CODE_SUCCESS, (uint16_t)value);
// }

// void pcbaKeyTestHandle(uint8_t value)
// {
//     pcbaKeyTestAck(PCBA_TEST_CODE_SUCCESS, value);
// }

// uint8_t pcbaTestGetHandleStep(void)
// {
//     return produceTestHandle.step;
// }

void produceTestLoop(void)
{
//     uint8_t len;
//     uint8_t buffer[DEVICE_SN_LEN_MAX];
//     uint16_t value;
//     pcba_test_packet_t response_packet;
// #if (UART_PRINTF_ENABLE)
//     return;
// #endif
//     if (!isEmptyUser(false) || !test_enable)
//     {
//         if (test_enable)
//         {
//             produceTestHwConfig(0);
//         }
//         return;
//     }
//     if (true == timerTimeOut(test_allow_time) && !testing && !isProduceReboot())
//     {
//         produceTestHwConfig(0);
//     }
//     if (produceTestPacketDecode(&response_packet))
//     {
//         setTimeOut(SYSTEM_TIME_OUT_ID, WORK_TIME_OUT_VAULE);
//         if (PCBA_TEST_FRAME_HEADER == response_packet.header)
//         {
//             if (response_packet.cmd != PCBA_TEST_CMD_TEST_MODE && !testing && !isProduceReboot())
//                 return;

//             produceTestHandle.step = PCBA_TEST_STEP_IDLE;

//             switch (response_packet.cmd)
//             {
//             case PCBA_TEST_CMD_TEST_MODE:
//                 if (IS_NUMBER_IN_RANGE(response_packet.buf[0], 1, 254))
//                 {
//                     setProduceReboot(true);
//                     produceTestHandleEvent_callback(EVENT_RESULT_PRODUCE_TEST, 0);
//                     pcbaBaseTestAck(response_packet.cmd, PCBA_TEST_CODE_SUCCESS);
//                     testing = 1;
//                 }
//                 else
//                 {
//                     pcbaBaseTestAck(response_packet.cmd, PCBA_TEST_CODE_FAIL);
//                 }

//                 break;

//             case PCBA_TEST_CMD_NFC:
//                 produceTestHandleEvent_callback(EVENT_RESULT_NFC_TEST, 0);
//                 pcbaCardTestAck(PCBA_TEST_CODE_SUCCESS_INCEPT);
                
//                 break;
//             case PCBA_TEST_CMD_KEY_BOARD:
//                 setProduceReboot(true);
//                 if (response_packet.len <= (KEY_CNT + 1))
//                 {
//                     memcpy((uint8_t *)(&produceTestHandle.keyBoard.pressReport), response_packet.buf, response_packet.len);
//                     produceTestHandle.keyBoard.step = 0;
//                     produceTestHandle.step = PCBA_TEST_STEP_KEY_BOARD;
// #if (Enabled == PRINTF_TEST)
//                     OB_LOGD(TAG, "key board tab[%u]: ", produceTestHandle.keyBoard.pressReport);
//                     OB_LOGD_DUMP(&produceTestHandle.keyBoard.tab[0], KEY_CNT);
// #endif
//                     produceTestHandleEvent_callback(EVENT_RESULT_KEY_BOARD_TEST, 0);
//                     pcbaKeyBoardTestAck(PCBA_TEST_CODE_SUCCESS_INCEPT, KEY_NULL);
//                     keyboard_test_mode(1);
//                 }
//                 else
//                 {
//                     pcbaKeyBoardTestAck(PCBA_TEST_CODE_FAIL, KEY_NULL);
//                 }
//                 break;
//             case PCBA_TEST_CMD_GPIO:
//                 produceTestHandle.step = PCBA_TEST_STEP_IO;
//                 if (produceGpioTestHandle(response_packet.buf, response_packet.len))
//                 {
//                     pcbaBaseTestAck(response_packet.cmd, PCBA_TEST_CODE_SUCCESS);
//                 }
//                 else
//                 {
//                     pcbaBaseTestAck(response_packet.cmd, PCBA_TEST_CODE_FAIL);
//                 }
//                 break;
//             case PCBA_TEST_CMD_SELF_TEST:
//                 // batteryTaskScan(0);
//                 // value = batteryTaskReadVoltage();
//                 value = TestBatteryVoltage();
//                 produceSelfTestResultAck(PRODUCE_TEST_CODE_SUCCESS, value, is_spi_flash_test(), is_rtc_test());
//                 break;
//             case PCBA_TEST_CMD_WRITE_SN:
//                 setTimeOut(SYSTEM_TIME_OUT_ID, WORK_TIME_OUT_VAULE);
//                 if (writeProduceSn(response_packet.buf[0], &response_packet.buf[1], (response_packet.len - 1)))
//                 {
//                     produceTestHandle.step = PCBA_TEST_STEP_WRITE_SN;
//                     pcbaBaseTestAck(response_packet.cmd, PCBA_TEST_CODE_SUCCESS);
//                 }
//                 else
//                 {
//                     pcbaBaseTestAck(response_packet.cmd, PCBA_TEST_CODE_FAIL);
//                 }
//                 break;
//             case PCBA_TEST_CMD_READ_SN:
//                 setTimeOut(SYSTEM_TIME_OUT_ID, WORK_TIME_OUT_VAULE);
//                 if (readProduceSn(response_packet.buf[0], buffer, (&len)))
//                 {
//                     produceTestHandle.step = PCBA_TEST_STEP_READ_SN;
//                     pcbaTestSendAckCmd(response_packet.cmd, PCBA_TEST_CODE_SUCCESS, buffer, len);
//                 }
//                 else
//                 {
//                     pcbaBaseTestAck(response_packet.cmd, PCBA_TEST_CODE_FAIL);
//                 }
//                 break;
//             case PCBA_TEST_CMD_SLEEP:
//                 produceTestHandle.step = PCBA_TEST_STEP_SLEEP;
//                 setProduceReboot(true);
//                 produceTestHandleEvent_callback(EVENT_RESULT_SLEEP, 0);
//                 pcbaBaseTestAck(response_packet.cmd, PCBA_TEST_CODE_SUCCESS);
//                 appm_stop_advertising();
//                 appm_delete_advertising(); //鍒犻櫎骞挎挱
//                 hal_wdt_disable();
//                 resetKeyInit();
//                 // hallInit();

//                 // cpu_reset();
//                 break;
//             case PCBA_TEST_CMD_MOTOR:
//                 hall_pullhigh();
//                 if (response_packet.buf[0] < PCBA_MOTOR_TEST_CNT)
//                 {
//                     produceTestHandle.step = PCBA_TEST_STEP_MOTOR;
//                     produceTestHandleEvent_callback(EVENT_RESULT_MOTOR_TEST, response_packet.buf[0]);
//                     pcbaMotorTestAck(PCBA_TEST_CODE_SUCCESS_INCEPT, 0);
//                 }
//                 else
//                 {
//                     pcbaMotorTestAck(PCBA_TEST_CODE_FAIL, 0);
//                 }
//                 break;
//             case PCBA_TEST_CMD_VERSION:
//                 pcbaVersionTestAck(PCBA_TEST_CODE_SUCCESS);
//                 break;
//             case PCBA_TEST_CMD_BLUE_TEST:
//                 //5A 0C 21 05 00 64 4F 42 5F 42 4C 45 30 30 30 6D 0D 0A
//                 //05 00--------------------------------5鍖?
//                 //64-----------------------------------100%鎴愬姛鐜?
//                 //4F 42 5F 42 4C 45 30 30 30-----------OB_BLE000
//                 ble_adc_name_set_test(&response_packet.buf[3], 9);
//                 appm_stop_advertising();   //鍏堝仠姝㈠箍鎾?
//                 appm_delete_advertising(); //鍒犻櫎骞挎挱
//                 ke_timer_set(APP_PERIOD_TIMER, TASK_APP, 10);
//                 //5绉掕秴鏃?
//                 break;
//             case PCBA_TEST_CMD_VOICE:
//                 keyBoardTaskSleep(KEY_TYPE_KEY_BOARD);
//                 PLAYER_LIST_ADD(response_packet.buf[0]);
//                 keyBoard_testmode(1);
//                 pcbaBaseTestAck(PCBA_TEST_CMD_VOICE, PCBA_TEST_CODE_SUCCESS);
//                 break;
//             case PCBA_TEST_CMD_BATCALI:
//                 setBatterycali(0,1);
//                 pcbaBatteryCaliAck(&response_packet.buf[0]);
//                 break;
//             case PCBA_TEST_CMD_READ_BATCALI:
//                 pcbaReadBatteryCaliAck(PCBA_TEST_CODE_SUCCESS);
//                 break;
//             case PCBA_TEST_CMD_KEY_TEST:
//                 if (response_packet.buf[0] < PCBA_KEY_TEST_CNT)
//                 {
//                     produceTestHandle.step = PCBA_TEST_STEP_KEY;
//                     produceTestHandleEvent_callback(EVENT_RESULT_KEY_TEST, response_packet.buf[0]);
//                     pcbaKeyTestAck(PCBA_TEST_CODE_SUCCESS_INCEPT, 0);
//                 }
//                 else
//                 {
//                     pcbaKeyTestAck(PCBA_TEST_CODE_FAIL, 0);
//                 }
//                 break;
//             default:
//                 break;
//             }
//         }
//         else
//         {
//             para_wr_comm_frame_t para;
//             send_resp_any();
//             memcpy(&para, &response_packet, sizeof(pcba_test_packet_t));
//             testing = 1;
//             switch (para.cmd)
//             {
//             case KDS_CMD_MODE_SEL: // OK
//                 hw_test_mode_handler(para.buff[0]);
//                 // if (1 == para.buff[0])
//                 // {    
//                 //     setUserParameter(USER_PARA_INIT_MOTOR_RESET_ID, Disabled);//涓嬫鍚姩灏嗚繘琛岃嚜鏍″噯
//                 //     baseEventPush(Q_HANDLE_SIG, EVENT_RESULT_DEV_PARA_MODE);
//                 // }
//                 // else if (0 == para.buff[0])
//                 //     baseEventPush(Q_HANDLE_SIG, EVENT_RESULT_DEV_NORMAL_MODE);
//                 // else if (2 == para.buff[0])
//                 //     baseEventPush(Q_HANDLE_SIG, EVENT_RESULT_AGING_TEST);
//                 break;
//             case KDS_CMD_BLE_SN: // OK
//                 hw_test_ble_sn_set_handler(&para.buff[0], para.len - 1);
//                 break;
//             case KDS_CMD_BLE_KEY:
//                 hw_test_ble_key_set_handler(&para.buff[0], para.len - 1);
//                 break;
//             case KDS_CMD_SN:
//                 if (SUB_CMD_SN_READ == para.buff[0]) // OK
//                 {
//                     hw_test_sn_read_handler(DEVICE_SN_KDS);
//                 }
//                 else if (SUB_CMD_SN_SET == para.buff[0]) // OK
//                 {
//                     hw_test_sn_set_handler(DEVICE_SN_KDS, &para.buff[1], para.len - 2);
//                 }
//                 else if (SUB_CMD_ESN_READ == para.buff[0]) // OK
//                 {
//                     hw_test_ble_sn_read_handler();
//                 }
//                 break;
//             case KDS_CMD_MAC: // OK
//                 hw_test_mac_read_handler();
//                 break;
//             case KDS_CMD_GET_RESULT:
//                 hw_test_send_produce_result();
//                 break;
//             case KDS_CMD_BATTERY:
//                 hw_test_send_battery();
//                 break;
//             case KDS_CMD_RESET:
//                 hw_test_ack_general(KDS_CMD_RESET, OB_PARA_SUCCES);
//                 system_init(true);
//                 clearDeviceTestFlagAllBit();
//                 resetDeviceTestResult();
//                 break;
//             case KDS_CMD_SLEEP:
//                 hw_test_ack_general(KDS_CMD_SLEEP, OB_PARA_SUCCES);
//                 setallowMotorTest(true);
//                 baseEventPush(Q_TIME_OUT_SIG, 0);
//                 break;
//             case KDS_CMD_BLOCKKEY:
//                 if (para.buff[0] == 0x04)
//                 {
//                     hw_test_ack_general(KDS_CMD_BLOCKKEY, OB_PARA_SUCCES);
//                     block_hotkey(true);
//                 }
//                 else if(0x05 == para.buff[0])
//                 {
//                     hw_test_ack_general(KDS_CMD_BLOCKKEY, OB_PARA_SUCCES);
//                     block_hotkey(false);
//                 }
//                 break;
//             case KDS_CMD_WRITEINFO:
//                 hw_test_writeInfo(&para.buff[0]);
//                 break;
//             case KDS_CMD_READINFO:
//                 hw_test_readInfo();
//                 break;
//             case KDS_CMD_VERSION:
//                 hw_test_sendVersion(para.buff[0]);
//                 break;
//             default:
//                 break;
//             }
//         }
//     }
}

// void exit_produce_test(void)
// {
//     testing = 0;
//     produceTestHwConfig(0);
// }

// uint8_t isTestEnable(void)
// {
//     return true;
// }

// uint8_t isTesting(void)
// {
//     return testing;
// }
