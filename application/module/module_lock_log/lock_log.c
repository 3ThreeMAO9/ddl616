#include "lock_log.h"
#include "lock_log_flash.h"
#include "lock_log_def.h"
#include <string.h>
#include "system_timer.h"
#include "config.h"
#include "hal_rtc.h"
#include "user.h"

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

    // 无管理员（空锁）时只挡开锁/报警记录，用户、钥匙等操作记录照常保存
    if (((KIOT_TM_P_RECORD_EVENT_TYPE_CAO_ZUO_JI_LU == record_event_type)
            && (KIOT_TM_P_RECORD_OPERATION_TYPE_KAI_SUO_JI_LU == type))
        || (KIOT_TM_P_RECORD_EVENT_TYPE_BAO_JING_JI_LU == record_event_type))
    {
        if (isEmptyKey(false))
        {
            return;
        }
    }

    memset(&item, 0xFF, sizeof(lock_log_item_t));

    item.record.p_timestamp = timestamp_check(hal_get_rtc_utc_time());  //日志存储UTC时间戳
    item.record.p_record_event_type = record_event_type;
    item.record.event_type.p_record_operation_type = type;
    item.record.param1.p_record_unlock_type = param1;
    item.record.p_key_id = param2;

    lock_log_flash_save((uint8_t *)&item);
}

// 该记录是否命中查询条件
static uint8_t lock_log_record_match(const lock_log_item_t *item, uint8_t event_type,
                                     uint32_t timestamp, uint8_t direction)
{
    if ((KIOT_TM_P_RECORD_EVENT_TYPE_QUAN_BU != event_type)
        && (item->record.p_record_event_type != event_type))
    {
        return 0;
    }

    if (KIOT_TM_P_RECORD_CURSOR_DIRECTION_JI_YU_MOU_GE_SHI_KE_HUO_QU_GUO_QU_DE_SHU_JU == direction)
    {
        return (item->record.p_timestamp < timestamp);      // 过去：早于基准时刻
    }
    return (item->record.p_timestamp > timestamp);          // 未来：晚于基准时刻
}

// 单次查询最多返回多少条：协议 p_record_cursor_count 上限 20（raw 200 字节 / 8 字节）
#define LOCK_LOG_QUERY_MAX_CNT      (20)

/* 单遍查询的收集缓冲：按 (时间戳, 序号) 升序存候选记录
 * 单遍遍历日志区，最多留 count 条（过去留最新的、未来留最旧的）。
 * 用 static 是为了不占任务栈；uart 单线程调用，非重入 */
static lock_log_item_t  s_query_sel[LOCK_LOG_QUERY_MAX_CNT];
static uint8_t          s_query_n;

// 按 (时间戳, 序号) 比较：>0 表示 a 更新
static int32_t lock_log_item_cmp(const lock_log_item_t *a, const lock_log_item_t *b)
{
    if (a->record.p_timestamp != b->record.p_timestamp)
    {
        return (a->record.p_timestamp > b->record.p_timestamp) ? 1 : -1;
    }
    if (a->write_seq != b->write_seq)
    {
        return (a->write_seq > b->write_seq) ? 1 : -1;
    }
    return 0;
}

// 把 item 按升序插入 s_query_sel；满了以后按 keep_newest 丢掉另一端
//   keep_newest=1：只留最新的 count 条（查"过去的数据"）
//   keep_newest=0：只留最旧的 count 条（查"未来的数据"）
static void lock_log_query_add(const lock_log_item_t *item, uint8_t count, uint8_t keep_newest)
{
    uint8_t pos = 0;

    if (s_query_n >= count)
    {
        if (keep_newest)
        {
            if (lock_log_item_cmp(item, &s_query_sel[0]) <= 0)          // 不比手上最旧的更新 -> 丢
            {
                return;
            }
        }
        else
        {
            if (lock_log_item_cmp(item, &s_query_sel[s_query_n - 1]) >= 0)  // 不比手上最新的更旧 -> 丢
            {
                return;
            }
        }
    }

    // 找升序插入位置
    while ((pos < s_query_n) && (lock_log_item_cmp(item, &s_query_sel[pos]) > 0))
    {
        pos++;
    }

    if (s_query_n >= count)
    {
        // 已满：先挤掉一端，再插入（插入位置随之平移）
        if (keep_newest)
        {
            memmove(&s_query_sel[0], &s_query_sel[1],
                    (uint16_t)(s_query_n - 1) * sizeof(lock_log_item_t));
            s_query_n--;
            if (pos > 0)
            {
                pos--;
            }
        }
        else
        {
            s_query_n--;                                                // 丢最新的一条
            if (pos > s_query_n)
            {
                pos = s_query_n;
            }
        }
    }

    if (pos < s_query_n)
    {
        memmove(&s_query_sel[pos + 1], &s_query_sel[pos],
                (uint16_t)(s_query_n - pos) * sizeof(lock_log_item_t));
    }
    s_query_sel[pos] = *item;
    s_query_n++;
}

// 按条件取历史记录 raw（0x89 应答用）
// 单遍遍历日志区（每条只读一次 flash），返回的这批按时间正序（旧 -> 新）：
//   direction=过去：紧邻基准时刻之前的 count 条（不足则给到最早）
//   direction=未来：紧邻基准时刻之后的 count 条（不足则给到最新）
uint16_t lock_log_get_records_raw(uint8_t event_type, uint32_t timestamp, uint8_t count,
                                  uint8_t direction, uint8_t* buf, uint16_t max_len)
{
    lock_log_item_t item;
    uint32_t index = 0;
    uint32_t seq_min;
    uint32_t seq_max;
    uint32_t ts_oldest = 0;         // 有效范围内最早一条的时间戳（定位用）
    uint32_t ts_newest = 0;         // 有效范围内最新一条的时间戳（定位用）
    uint32_t cnt_in_range = 0;      // 有效范围内的记录条数（定位用）
    uint8_t keep_newest;
    uint8_t i;

    if ((buf == NULL) || (count == 0))
    {
        return 0;
    }

    // 放得下几条就取几条（单条 8 字节，raw 上限由调用方传入）
    while ((count > LOCK_LOG_QUERY_MAX_CNT)
           || ((uint16_t)count * sizeof(lock_log_record_t) > max_len))
    {
        count--;
    }
    if (count == 0)
    {
        return 0;
    }

    seq_min = get_log_range_min_seq();      // 有效记录范围内最小编号
    seq_max = get_log_seq_max();            // 最新记录编号
    keep_newest = (KIOT_TM_P_RECORD_CURSOR_DIRECTION_JI_YU_MOU_GE_SHI_KE_HUO_QU_GUO_QU_DE_SHU_JU == direction);

    s_query_n = 0;

    while (lock_log_flash_read_next(&index, &item))
    {
        // 只认有效范围内(最新 600 条)的记录
        if ((item.write_seq < seq_min) || (item.write_seq > seq_max))
        {
            continue;
        }

        // 定位用：先把范围内的记录条数和时间范围记下来（不看是否命中过滤条件）
        if (cnt_in_range == 0)
        {
            ts_oldest = item.record.p_timestamp;
            ts_newest = item.record.p_timestamp;
        }
        else
        {
            if (item.record.p_timestamp < ts_oldest)
            {
                ts_oldest = item.record.p_timestamp;
            }
            if (item.record.p_timestamp > ts_newest)
            {
                ts_newest = item.record.p_timestamp;
            }
        }
        cnt_in_range++;

        if (!lock_log_record_match(&item, event_type, timestamp, direction))
        {
            continue;
        }
        lock_log_query_add(&item, count, keep_newest);
    }

    for (i = 0; i < s_query_n; i++)
    {
        memcpy(buf + i * sizeof(lock_log_record_t), &s_query_sel[i].record, sizeof(lock_log_record_t));
    }

    OB_LOGD(TAG, "records raw: event_type=%u, ts=%u, dir=%u, got=%u, seq[%u,%u], rec %u ts[%u,%u]",
            event_type, timestamp, direction, s_query_n, seq_min, seq_max,
            cnt_in_range, ts_oldest, ts_newest);

    // 定位日志：0 条时说清是"锁里没有记录"还是"基准/方向和记录对不上"
    if (s_query_n == 0)
    {
        if (cnt_in_range == 0)
        {
            OB_LOGW(TAG, "no record: lock has 0 record in range, base=%u, dir=%u, event_type=%u",
                    timestamp, direction, event_type);
        }
        else
        {
            OB_LOGW(TAG, "no record hit: %u rec(s) ts[%u,%u], base=%u, dir=%u, event_type=%u (newest is %s base)",
                    cnt_in_range, ts_oldest, ts_newest, timestamp, direction, event_type,
                    (ts_newest < timestamp) ? "older than" : "newer than");
        }
    }

    return (uint16_t)s_query_n * sizeof(lock_log_record_t);
}

