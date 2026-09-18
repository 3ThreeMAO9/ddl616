#include "task_nfc.h"
#include "msg_protocol.h"
#include "protocol_func_card.h"
#include "user.h"
#include "event.h"

#define OB_LOG_LEVEL OB_LOG_LEVEL_NONE
#include "ob_log.h"
#define TAG "task_nfc"

/***************Variable***************/
static nfc_task_driver_t nfc_task_driver;

// ------------------------------------------
static void nfc_task_callback(nfc_event_t *event)
{
    OB_LOGD(TAG, "event: size[%u]", event->size);
    OB_LOGD_DUMP(event->card_id, event->size);
    if (event->valid)
    {
        switch (event->state)
        {
        case NFC_STATE_VERIFY:
            OB_LOGI(TAG, "verify success");
            if (isValidUserCard(&(event->para) ,event->card_id)){
                userHandleEventPush(EVENT_RESULT_SUCCESS_VERIFY_USER, event->para);
            }
            else{
                userHandleEventPush(EVENT_RESULT_FAIL_INVALID, 0);
            }
            break;
        case NFC_STATE_REGISTER:
            OB_LOGI(TAG, "register success");
            if (!isValidUserCard(&(event->para), event->card_id))
            {
                uint16_t uu_id = 0;
                if (addUserCard(event->card_id, (&(uu_id))))
                {
                    OB_LOGI(TAG, "->uu_id [%ld]", uu_id);
                    userHandleEventPush(EVENT_RESULT_SUCCESS_ADD, event->para);
                }
                else
                {
                    userHandleEventPush(EVENT_RESULT_FAIL_INVALID, 0);
                }
            }
            else
            {
                userHandleEventPush(EVENT_RESULT_FAIL_CARD_REPEAT, 0);
            }
            break;
        case NFC_STATE_FUNCTION:
            OB_LOGI(TAG, "state function");
            break;
        default:
            OB_LOGE(TAG, "[%s] not default", __func__);
            break;
        }
    }
    else
    {
        switch (event->state)
        {
        case NFC_STATE_VERIFY:
            OB_LOGI(TAG, "verify fail");
            userHandleEventPush(EVENT_RESULT_FAIL_INVALID, 0);
            break;
        case NFC_STATE_REGISTER:
            OB_LOGI(TAG, "register fail");
            userHandleEventPush(EVENT_RESULT_FAIL_INVALID, 0);
            break;
        case NFC_STATE_FUNCTION:
            OB_LOGI(TAG, "state function");
            if (event->sector_valid)
            {
                OB_LOGD_DUMP(event->sector_pt, 48);
                // uart_msg_nfc_read_sector(EVENT_CODE_CARD_READ_SECTOR, event->card_id, event->sector_pt);
            }
            break;
        default:
            OB_LOGE(TAG, "[%s] not default", __func__);
            break;
        }
        OB_LOGD(TAG, "Fail: The card is invalid");
    }
}

static void set_nfc_attr(const nfc_attribute_t *attr)
{
    if (NULL == nfc_task_driver.io->set_attr)
    {
        return;
    }

    nfc_task_driver.io->set_attr(&nfc_task_driver.attr);
}

static void set_nfc_encrypt_type(uint8_t encrypt_type)
{

    nfc_task_driver.attr.encrypt_type = encrypt_type;
    set_nfc_attr(&nfc_task_driver.attr);
}

void nfc_task_scan_get_tick(uint32_t time)
{
    nfc_task_driver.io->scan_get_tick(time);
}

void nfc_task_set_encrypt(uint8_t flag)
{
    // Write flash
    // ...

    if (TRUN_ON == flag)
    {
        set_nfc_encrypt_type(NFC_ENCRYPT_DEFAULT);
    }
    else
    {
        set_nfc_encrypt_type(NFC_ENCRYPT_NONE);
    }
}

void nfc_task_init(void)
{
    OB_LOGD(TAG, "Init");
    nfc_task_driver.io = bsp_nfc_get_driver(nfc_task_callback);

    if (NULL == nfc_task_driver.io->init)
    {
        return;
    }

    memset((uint8_t *)(&nfc_task_driver.attr), 0, sizeof(nfc_attribute_t));

    if (!nfc_task_driver.io->init(&nfc_task_driver.attr))
    {
        return;
    }

    set_nfc_encrypt_type(NFC_ENCRYPT_DEFAULT);
    nfc_task_set_state(NFC_STATE_VERIFY);
}

void nfc_task_loop(void)
{
    if (NULL == nfc_task_driver.io->loop)
    {
        return;
    }

    nfc_task_driver.io->loop();
}

uint16_t nfc_task_is_wake(void)
{
    if (NULL == nfc_task_driver.io->is_wake)
    {
        return WAKE_SOURCE_NULL;
    }

    if (nfc_task_driver.io->is_wake())
    {
        return WAKE_SOURCE_NFC;
    }

    return WAKE_SOURCE_NULL;
}

void nfc_task_set_state(uint8_t state)
{
    uint8_t mode;

    if (NULL == nfc_task_driver.io->set_mode)
    {
        return;
    }
    OB_LOGD(TAG, "set state[%u]", state);
    nfc_task_driver.attr.state = state;

    nfc_task_driver.attr.encrypt = 0;
    nfc_task_driver.attr.block_sel = 0;
    nfc_task_driver.attr.sector_id = 0;

    switch (state)
    {
    case NFC_STATE_SLEEP:
        nfc_task_driver.attr.encrypt = 0;
        nfc_task_driver.attr.sector_id = NFC_USER_CARD_SECTOR_ID;
        mode = NFC_MODE_IDLE; // 根据硬件支持LPCD唤醒或ADC唤醒中的一种
        break;
    case NFC_STATE_VERIFY:
        nfc_task_driver.attr.encrypt = 0;
        nfc_task_driver.attr.sector_id = NFC_USER_CARD_SECTOR_ID;
        mode = NFC_MODE_CHECK; // 根据硬件支持LPCD唤醒或ADC唤醒中的一种
        break;
    case NFC_STATE_REGISTER:
        nfc_task_driver.attr.encrypt = 1;
        nfc_task_driver.attr.sector_id = NFC_USER_CARD_SECTOR_ID;
        mode = NFC_MODE_SCAN; // 采用定时扫描或LPCD中断事件触发
        break;
    case NFC_STATE_CARRIER:
        mode = NFC_MODE_CARRIER; // 开启天线，用于测试模式
        break;
    case NFC_STATE_FUNCTION:
        mode = NFC_MODE_FUNC; // 采用定时扫描或LPCD中断事件触发
        nfc_task_driver.attr.block_sel = 0x07;
        nfc_task_driver.attr.sector_id = NFC_FUNC_CARD_SECTOR_ID;
        /*if (is_empty_user_key(USER_ROLE_ALL, USER_KEY_TYPE_ALL)) {
            nfc_task_driver.attr.block_sel = 0x07;
            nfc_task_driver.attr.sector_id = NFC_FUNC_CARD_SECTOR_ID;
        }*/
        break;
    default:
        mode = NFC_MODE_DEEP_SLEEP;
        break;
    }

    set_nfc_attr(&nfc_task_driver.attr);

    nfc_task_driver.io->set_mode(mode);
}
