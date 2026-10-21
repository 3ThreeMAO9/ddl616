#include "event.h"
#include "event_queue.h"

#define OB_LOG_LEVEL OB_LOG_LEVEL_NONE
#include "ob_log.h"
#define TAG "event"

/***************Variable***************/


// ------------------------------------------
void baseEventPush(uint8_t event_type, uint8_t value)
{
    QEvent event;

    event.sig = event_type;
    event.dynamic_[0] = value;

    eventQueuePush(&event);
}

void handleEventPush(uint8_t handle, uint8_t value)
{
    QEvent event;

    event.sig = Q_HANDLE_SIG;
    event.dynamic_[0] = handle;
    event.dynamic_[1] = value;

    eventQueuePush(&event);
}

static void userHandleEventPushKey(uint8_t result, uint16_t user_sn, uint8_t key_type)
{
    QEvent event;

    event.sig = Q_USER_HANDLE_SIG;
    event.dynamic_[0] = result;
    event.dynamic_[1] = (uint8_t)(user_sn>>8);
    event.dynamic_[2] = (uint8_t)(user_sn&0xFF);
    event.dynamic_[3] = key_type;       // 钥匙类型（双重验证用它区分两次验证方式）

    eventQueuePush(&event);
}

void userHandleEventPush(uint8_t result, uint16_t user_sn)
{
    userHandleEventPushKey(result, user_sn, 0xFF);      // 0xFF = 无钥匙类型（不参与双重验证）
}

// 验证类事件：带验证来源
void userHandleEventPushType(uint8_t result, uint16_t user_sn, verify_src_t src)
{
    userHandleEventPushKey(result, user_sn, (uint8_t)src);
}

void nfcKeyEventPush(uint8_t* id, uint8_t size) {
    QEvent event;

    event.sig = Q_NFC_KEY_SIG;
    event.dynamic_[0] = size;
    memcpy(&event.dynamic_[1], id, size);

    eventQueuePush(&event);
}

void linkKeyEventPush(uint8_t* id, uint8_t size) {
    QEvent event;

    event.sig = Q_LINK_KEY_SIG;
    event.dynamic_[0] = size;
    memcpy(&event.dynamic_[1], id, size);

    eventQueuePush(&event);
}
