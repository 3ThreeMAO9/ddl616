#include "timestamp.h"
#define OB_LOG_LEVEL OB_LOG_LEVEL_DEBUG
#include "ob_log.h"
#define TAG "timestamp"
/***************Variable***************/


// ------------------------------------------

struct tm *timestamp_to_data_time(time_t timestamp)
{
    struct tm *time_info;

    time_info = localtime(&timestamp);
    if (time_info != NULL)
    {
        OB_LOGD(TAG, "time: %ld   %04d/%02d/%02d  %02d:%02d:%02d", timestamp, time_info->tm_year + 1900, time_info->tm_mon + 1, time_info->tm_mday, time_info->tm_hour, time_info->tm_min, time_info->tm_sec);
    }
    return time_info;
}
