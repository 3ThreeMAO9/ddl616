#ifndef _LOCK_LOG_H
#define _LOCK_LOG_H

#include "lock_log_def.h"
#include "lock_log_flash.h"


void lock_log_user_operation_add(uint8_t event_source, uint8_t event_code, uint8_t code_id);
void lock_log_user_program_add(uint8_t event_source, uint8_t event_code, uint8_t code_id);
void lock_log_user_alarm_add(uint32_t alarm_code, uint8_t reserved);

uint32_t lock_log_get_last_timestamp(void);
#endif // _LOCK_LOG_H
