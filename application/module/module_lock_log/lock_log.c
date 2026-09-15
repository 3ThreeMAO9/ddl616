#include "lock_log.h"
#include "lock_log_flash.h"
#include "lock_log_def.h"
#include "system_timer.h"
#include "config.h"
#include "hal_rtc.h"

#define OB_LOG_LEVEL OB_LOG_LEVEL_DEFAULT
#include "ob_log.h"
#define TAG "lock_log"

static uint32_t last_log_timestamp = 0;


static inline uint32_t timestamp_check(uint32_t timestamp)
{
    if(last_log_timestamp == timestamp)
    {
        last_log_timestamp = timestamp+1;
    }
    else
    {
        last_log_timestamp = timestamp;
    }
    OB_LOGW(TAG,"last_log_timestamp %08X",last_log_timestamp);
    return last_log_timestamp;
}

uint16_t key_item_id_get_extern_part(uint16_t id)
{
    return id;
}

uint32_t lock_log_get_last_timestamp(void)
{
    time_t log_timestamp = 0;
    log_timestamp = hal_get_rtc_to_utc_time(last_log_timestamp);
    return log_timestamp;
}

void lock_log_user_operation_add(uint8_t event_source, uint8_t event_code, uint8_t code_id)
{
    lock_log_item_t item;
    memset(&item, 0xFF, sizeof(lock_log_item_t));

    item.data.type = 0;
    item.data.operation.event_source = event_source;
    item.data.operation.event_code = event_code;
    item.data.operation.code_id = code_id;
    item.timestamp = timestamp_check(hal_get_rtc_utc_time());   //日志存储UTC时间戳

    lock_log_flash_save((uint8_t *)&item);
}


void lock_log_user_program_add(uint8_t event_source, uint8_t event_code, uint8_t code_id)
{
    lock_log_item_t item;
    memset(&item, 0xFF, sizeof(lock_log_item_t));

    item.data.type = 1;
    item.data.progarm.event_source = event_source;
    item.data.progarm.event_code = event_code;
    item.data.progarm.code_id = code_id;
    item.timestamp = timestamp_check(hal_get_rtc_utc_time());   //日志存储UTC时间戳

    lock_log_flash_save((uint8_t *)&item);
}


void lock_log_user_alarm_add(uint32_t alarm_code, uint8_t reserved)
{
    lock_log_item_t item;

    memset(&item, 0xFF, sizeof(lock_log_item_t));
    item.data.type = 2;
    item.data.alarm.alarm_code = *((uint32_t*)&alarm_code);
    item.data.alarm.reserved = reserved;
    item.timestamp = timestamp_check(hal_get_rtc_utc_time());   //日志存储UTC时间戳

    lock_log_flash_save((uint8_t *)&item);
}

