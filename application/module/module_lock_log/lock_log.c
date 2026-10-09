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

// 写一条历史记录（字段含义见 lock_log_def.h）
void lock_log_add_record(uint8_t record_event_type, uint8_t type, uint8_t param1, uint8_t param2)
{
    lock_log_item_t item;

    memset(&item, 0xFF, sizeof(lock_log_item_t));

    item.record.p_timestamp = timestamp_check(hal_get_rtc_utc_time());  //日志存储UTC时间戳
    item.record.p_record_event_type = record_event_type;
    item.record.event_type.p_record_operation_type = type;
    item.record.param1.p_record_unlock_type = param1;
    item.record.p_key_id = param2;

    lock_log_flash_save((uint8_t *)&item);
}

