#include "event_queue.h"

#define OB_LOG_LEVEL OB_LOG_LEVEL_NONE
#include "ob_log.h"
#define TAG "event_queue"

/***************Variable***************/
static event_queue_t event_queue;

// ------------------------------------------

void eventQueueInit(void)
{
    memset((uint8_t*)(&event_queue), 0, sizeof(event_queue_t));
}

void eventQueuePush(QEvent* event)
{
    OB_LOGD(TAG, "push event[%u, %u]", event->sig, event->dynamic_[0]);

    if (event_queue.cnt<EVENT_QUEUE_SIZE)
    {
        memcpy((uint8_t*)(&event_queue.queue[event_queue.rear_id].sig), (uint8_t*)(event), sizeof(QEvent));
        event_queue.rear_id++;
        event_queue.rear_id %= EVENT_QUEUE_SIZE;
        event_queue.cnt++;
    }
    else
    {
        OB_LOGE(TAG, "Error[%s]", "buffer is full");
    }
}

uint8_t eventQueuePop(QEvent* event)
{
    if (event_queue.cnt)
    {
        memcpy((uint8_t*)(event), (uint8_t*)(&event_queue.queue[event_queue.front_id].sig), sizeof(QEvent));
        event_queue.front_id++;
        event_queue.front_id %= EVENT_QUEUE_SIZE;
        event_queue.cnt--;

        OB_LOGD(TAG, "pop event[%u, %u]", event->sig, event->dynamic_[0]);

        return true;
    }

	return false;
}

