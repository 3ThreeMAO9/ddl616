/**
 * @file ota_timer.h
 * @author Dandjinh Deng (Dandjinh_Deng@on-bright.com)
 * @brief 
 * @version 0.1
 * @date 2022-12-15
 * 
 * @copyright Copyright (c) 2022  广州昂宝电子有限公司
 */

#ifndef _OTA_TIMER_H
#define _OTA_TIMER_H

#include <stdint.h>

#define OTA_REQUEST_DELAY_MS (200)
#define OTA_REQUEST_RETRY    (3)
#define OTA_WAKEUP_TIME_GET     (100)
#define OTA_WAKEUP_TIME_REDAY   (400)
#define OTA_WAKEUP_TIME_BLE     (10*600)
#define OTA_WAKEUP_TIME_BLE_SLAVE     (12*600)

#if(Enabled == PRINTF_EN)

#define xLOG(fmt, args...)   do { \
										EndPWMAudio(); \
										printf(fmt, ##args); \
									} while(0)
#else

#define xLOG(fmt, args...)
#endif

void ota_timer_loop(void);

void ota_timer_reset(void);

uint32_t ota_timer_start_request(uint16_t delay, uint8_t retry);
void ota_timer_request_reply(uint8_t reply);

void ota_timer_start_update(uint32_t size);

void ota_timer_receive_data(uint32_t offset,
    uint8_t *buf, uint16_t size);

#endif // _OTA_TIMER_H
