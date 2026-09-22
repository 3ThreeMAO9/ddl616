#include "key_event.h"
#include "event.h"
#include "user.h"
#include "parameter.h"
#include "utils.h"
#include "lock_log.h"
#include "task_sleep.h"
#include "task_key.h"
#include "led.h"
#include "flash_data.h"
#include "hal_rtc.h"

// #include "offlinekey_port.h"
// #include "offlinekey.h"
#define OB_LOG_LEVEL OB_LOG_LEVEL_NONE
#include "ob_log.h"
#define TAG "key_event"

/***************Variable***************/
static key_board_event_t keyBoardEvent;

/*****************Macro****************/
#define CLEAR_KEY_EVENT() memset((uint8_t *)(&keyBoardEvent), 0, sizeof(key_board_event_t));
#define CLEAR_KEY_EVENT_INPUT(X) memset((uint8_t *)(&keyBoardEvent.input[X]), 0, sizeof(key_board_input_t));
#define COPY_KEY_EVENT_INPUT()                                                                                         \
    {                                                                                                                  \
        memcpy((uint8_t *)(&keyBoardEvent.input[1]), (uint8_t *)(&keyBoardEvent.input[0]), sizeof(key_board_input_t)); \
        CLEAR_KEY_EVENT_INPUT(0);                                                                                      \
    }

// ------------------------------------------
void keyEventInit(void)
{
    CLEAR_KEY_EVENT();
}

static uint8_t keyEventCombineFunctionHandle(void)
{
    uint32_t value;

    value = arraysConvertNumber(keyBoardEvent.input[0].buffer, keyBoardEvent.input[0].len);

    if (208 == value)
    {
        baseEventPush(Q_HANDLE_SIG, EVENT_RESULT_RESET);
        return true;
    }
    // else if (111 == value)
    // {
    //     clear_activecode();
    //     return true;
    // }
    // else if (112 == value)
    // {
    //     uint8_t code[ACTIVECODE_LEN_MAX] = {6, 7, 8, 9, 1, 2};
    //     write_activecode_hash(code, ACTIVECODE_LEN_MAX);
    //     return true;
    // }
    // else if (114 == value)
    // {
    //     produce_info_t* info = (produce_info_t*)get_produce_info();
    //     OB_LOGW(TAG, "debug: flag=%u, locked=%u, activated=%u",
    //             info->activecode.flag, is_device_locked(), is_activated());
    //     return true;
    // }


    else if (116 == value)
    {
        baseEventPush(Q_HANDLE_SIG, EVENT_RESULT_ENTER_NET_CONFIG);
        return true;
    }
    // else if (117 == value)
    // {
    //     lock_log_flash_test(50 , 0 , 4);
    //     return true;
    // }
    // else if (118 == value)
    // {
    //     open_log_pr();
    //     return true;
    // }
    // else if (119 == value)
    // {
    //     lock_log_flash_clear();
    //     return true;
    // }

    if (isEmptyUser(false))
    {
        if (COMBINE_KEY_BOARD_AGING_TEST == value && !is_block_hotkey())
        {
            baseEventPush(Q_HANDLE_SIG, EVENT_RESULT_AGING_TEST);
            return true;
        }
        else if (COMBINE_KEY_BOARD_DEVICE_TEST == value && !is_block_hotkey())
        {
            baseEventPush(Q_HANDLE_SIG, EVENT_RESULT_DEVICE_TEST_STEP1);
            return true;
        }
        else if (COMBINE_KEY_BOARD_VERSION == value)
        {
            baseEventPush(Q_HANDLE_SIG, EVENT_RESULT_VERSION);
            return true;
        }
        else if (COMBINE_KEY_BOARD_TIME == value)
        {
            baseEventPush(Q_HANDLE_SIG, EVENT_RESULT_TIME);
            return true;
        }
#if (Enabled == WAKE_STAT_ENABLE)
        else if (COMBINE_KEY_BOARD_FINGER_WAKE == value)
        {
            play_finger_Wake_num();
            return true;
        }
        else if (COMBINE_KEY_BOARD_PASSWORD_WAKE == value)
        {
            play_password_Wake_num();
            return true;
        }
        else if (COMBINE_KEY_BOARD_NFC_WAKE == value)
        {
            play_card_Wake_num();
            return true;
        }
        else if (COMBINE_KEY_BOARD_STATE_WAKE == value)
        {
            play_doorstate_Wake_num();
            return true;
        }
        else if (COMBINE_KEY_BOARD_BT_WAKE == value)
        {
            play_ble_Wake_num();
            return true;
        }
        else if (COMBINE_KEY_BOARD_AUTOLOCK_WAKE == value)
        {
            play_autolock_Wake_num();
            return true;
        }
        else if (COMBINE_KEY_BOARD_RESET_WAKE == value)
        {
            play_resetkey_Wake_num();
            return true;
        }
        else if (COMBINE_KEY_BOARD_ALL_WAKE == value)
        {
            play_all_Wake_num();
            return true;
        }
        else if (COMBINE_KEY_BOARD_WAKE_CLR == value)
        {
            clear_all_Wake_num();
            return true;
        }
        else if (COMBINE_KEY_BOARD_WAKE_UP_CNT == value)
        {
            play_WakeUpCnt_num();
            return true;
        }
#endif
    }
    else
    {
        if (COMBINE_KEY_BOARD_VERSION == value)
        {
            baseEventPush(Q_HANDLE_SIG, EVENT_RESULT_VERSION);
            return true;
        }
        else if (COMBINE_KEY_BOARD_TIME == value)
        {
            baseEventPush(Q_HANDLE_SIG, EVENT_RESULT_TIME);
            return true;
        }
    }
    return false;
}

void keyEventVerifyAdmin(uint8_t key_value)
{
    uint16_t user_sn;

    if (KEY_CAN == key_value)
    {
        if (keyBoardEvent.input[0].len)
        {
            CLEAR_KEY_EVENT();
#if (Enabled == PRINTF_USER)
            OB_LOGD(TAG, "InputCode: input[%u]->", keyBoardEvent.input[0].len);
#endif
        }
        else
        {
            baseEventPush(Q_HANDLE_SIG, EVENT_RESULT_FAIL);
            CLEAR_KEY_EVENT();
        }
    }
    else if (KEY_OK == key_value)
    {
#if (Enabled == PRINTF_USER)
        if (keyBoardEvent.input[0].len)
        {
            OB_LOGD(TAG, "InputCode: input[%u]->", keyBoardEvent.input[0].len);
            OB_LOGD_DUMP(&keyBoardEvent.input[0].buffer[0], keyBoardEvent.input[0].len);
        }
#endif
        if (keyBoardEvent.input[0].len < USER_CODE_LEN_MIN)     // 密码输入过短
        {
#if (Enabled == PRINTF_PASSWORD)
            OB_LOGE(TAG, "handle fail: input len is too short[%u]", keyBoardEvent.input[0].len);
#endif
            if (keyBoardEvent.input[0].len == 0)
                baseEventPush(Q_HANDLE_SIG, EVENT_RESULT_VERIFY_INPUT_ERROR);
            else
                baseEventPush(Q_HANDLE_SIG, EVENT_RESULT_FAIL_TOO_SHORT);
            CLEAR_KEY_EVENT();
        }
        else
        {
            if (isValidUserCode(keyBoardEvent.input[0].buffer, keyBoardEvent.input[0].len, &user_sn, false, true, false) && (user_sn <= MASTER_USER_CODE_CNT))
            {
#if (Enabled == PRINTF_PASSWORD)
                OB_LOGD(TAG, "Verify success: password admin user[%u]", user_sn);
#endif
                userHandleEventPush(EVENT_RESULT_SUCCESS_VERIFY_ADMIN, user_sn);
                CLEAR_KEY_EVENT();
                return;
            }
            else
            {
#if (Enabled == PRINTF_PASSWORD)
                OB_LOGE(TAG, "verify fail: password user is invalid");
#endif
                userHandleEventPush(EVENT_RESULT_FAIL_INVALID, 0);
            }
            CLEAR_KEY_EVENT();
        }
    }
    else if (key_value < KEY_CNT)
    {
        if (keyBoardEvent.input[0].len < DUMMY_USER_CODE_LEN_MAX)
        {
            keyBoardEvent.input[0].buffer[keyBoardEvent.input[0].len] = key_value;
            keyBoardEvent.input[0].len++;
#if (Enabled == PRINTF_USER)
            if (keyBoardEvent.input[0].len)
            {
                OB_LOGD(TAG, "InputCode: input[%u]->", keyBoardEvent.input[0].len);
                OB_LOGD_DUMP(&keyBoardEvent.input[0].buffer[0], keyBoardEvent.input[0].len);
            }
#endif
        }
        else
        {
            // handle fail
#if (Enabled == PRINTF_PASSWORD)
            OB_LOGE(TAG, "handle fail: input is too long");
#endif
            baseEventPush(Q_HANDLE_SIG, EVENT_RESULT_FAIL_TOO_LONG);
            CLEAR_KEY_EVENT();
        }
    }
}

void keyEventVerifyUser(uint8_t key_value)
{
    uint16_t user_sn;

    if (KEY_CAN == key_value)
    {
        if (keyBoardEvent.input[0].len)
        {
            CLEAR_KEY_EVENT();
#if (Enabled == PRINTF_USER)
            OB_LOGD(TAG, "InputCode: input[%u]->", keyBoardEvent.input[0].len);
#endif
        }
        else
        {
            baseEventPush(Q_HANDLE_SIG, EVENT_RESULT_FAIL);
            CLEAR_KEY_EVENT();
        }
    }
    else if (KEY_OK == key_value)
    {
#if (Enabled == PRINTF_USER)
        if (keyBoardEvent.input[0].len)
        {
            OB_LOGD(TAG, "InputCode: input[%u]->", keyBoardEvent.input[0].len);
            OB_LOGD_DUMP(&keyBoardEvent.input[0].buffer[0], keyBoardEvent.input[0].len);
        }
#endif
        if (keyBoardEvent.input[0].len < USER_CODE_LEN_MIN)     // 密码输入过短
        {
            if (false == keyEventCombineFunctionHandle())
            {
#if (Enabled == PRINTF_PASSWORD)
                OB_LOGE(TAG, "handle fail: input len is too short[%u]", keyBoardEvent.input[0].len);
#endif
                if (keyBoardEvent.input[0].len == 0)
                    baseEventPush(Q_HANDLE_SIG, EVENT_RESULT_VERIFY_INPUT_ERROR);
                else
                    baseEventPush(Q_HANDLE_SIG, EVENT_RESULT_FAIL_TOO_SHORT);
            }
            CLEAR_KEY_EVENT();
        }
        else
        {
            if (isValidUserCode(keyBoardEvent.input[0].buffer, keyBoardEvent.input[0].len, &user_sn, true, true, true))
            {
                // 判断是否处于待激活
                if (is_device_locked() && (keyBoardEvent.input[0].len == ACTIVECODE_LEN_MAX))
                {
                    if (verify_activecode(keyBoardEvent.input[0].buffer, keyBoardEvent.input[0].len))
                    {
                        OB_LOGI(TAG, "activecode verify OK");
                        baseEventPush(Q_HANDLE_SIG, EVENT_RESULT_ACTIVECODE_SUCCESS);
                        CLEAR_KEY_EVENT();
                        return;
                    }
                }


#if (Enabled == PRINTF_PASSWORD)
                OB_LOGI(TAG, "verify success: password user[%u]", user_sn);
#endif
                if (Enabled == readUserParameter(USER_PARA_VACATION_MODE_ID))
                {
                    if (user_sn <= MASTER_USER_CODE_CNT)
                    {
                        setUserParameter(USER_PARA_VACATION_MODE_ID, Disabled);
                        userHandleEventPush(EVENT_RESULT_SUCCESS_VERIFY_USER, user_sn);
                    }
                    else
                    {
                        userHandleEventPush(EVENT_RESULT_VACATION_MODE_FAIL, 0);
                    }
                }
                else
                {
                    userHandleEventPush(EVENT_RESULT_SUCCESS_VERIFY_USER, user_sn);
                }
            }
            else
            {
#if (Enabled == PRINTF_PASSWORD)
                OB_LOGE(TAG, "verify fail: password user is invalid");
#endif
                userHandleEventPush(EVENT_RESULT_FAIL_INVALID, 0);
            }
            CLEAR_KEY_EVENT();
        }
    }
    else if (key_value < KEY_CNT)
    {
        if (keyBoardEvent.input[0].len < DUMMY_USER_CODE_LEN_MAX)
        {
            keyBoardEvent.input[0].buffer[keyBoardEvent.input[0].len] = key_value;
            keyBoardEvent.input[0].len++;
#if (Enabled == PRINTF_USER)
            if (keyBoardEvent.input[0].len)
            {
                OB_LOGD(TAG, "InputCode: input[%u]->", keyBoardEvent.input[0].len);
                OB_LOGD_DUMP(&keyBoardEvent.input[0].buffer[0], keyBoardEvent.input[0].len);
            }
#endif
        }
        else
        {
            // handle fail
#if (Enabled == PRINTF_PASSWORD)
            OB_LOGE(TAG, "handle fail: input is too long");
#endif
            baseEventPush(Q_HANDLE_SIG, EVENT_RESULT_FAIL_TOO_LONG);
            CLEAR_KEY_EVENT();
        }
    }
}

static uint8_t isSameInputCode(void)
{
    if (keyBoardEvent.input[0].len == keyBoardEvent.input[1].len)
    {
        if (compare_arrays(keyBoardEvent.input[0].buffer, keyBoardEvent.input[1].buffer, keyBoardEvent.input[0].len))
        {
            return true;
        }
    }

    return false;
}

static void codeHandle(uint8_t handle_code, uint8_t input_cnt)
{
    uint8_t result = EVENT_RESULT_FAIL;
    uint16_t user_sn;
    uint16_t user_ble_sn;
    user_time_t parameter;

    // 本地密码录入 user_id = 0xff;为空
    parameter.user_id = 0xff;
    parameter.user_policy = USER_POLICY_PERMANENT;
    parameter.key_urgent = KEY_URGENT_NORMAL;
    parameter.timestamp = hal_get_rtc_time();

#if (Enabled == PRINTF_PASSWORD)
    OB_LOGD(TAG, "%s  input_cnt: %d", __FUNCTION__, input_cnt);
#endif
    if (input_cnt)
    {
        if (isSameInputCode())
        {
            if ((isValidUserCode(keyBoardEvent.input[0].buffer, keyBoardEvent.input[0].len, &user_sn, true, false, false)) && (0 != user_sn))
            {
#if (Enabled == PRINTF_PASSWORD)
                OB_LOGD(TAG, "user sn[%u]: len[%u]", user_sn, keyBoardEvent.input[0].len);
#endif
                if ((CODE_HANDLE_ADD == handle_code) || (CODE_HANDLE_CHANGE_MASTER == handle_code))
                {
                    result = EVENT_RESULT_FAIL_REPEAT;
                }
                CLEAR_KEY_EVENT();
            }
            else
            {
                switch (handle_code)
                {
                case CODE_HANDLE_ADD:
                    if (true == addUserCode(keyBoardEvent.input[0].buffer, keyBoardEvent.input[0].len, USER_TYPE_PERMANENT_CODE, &user_sn, &parameter))
                    {
                        if (true == getUserFlag(&user_ble_sn, USER_TYPE_PERMANENT_CODE, user_sn - 1))
                        {
                            OB_LOGW(TAG, "user_sn  %0ld user_ble_sn %0ld", user_sn, user_ble_sn);
                        }
                        // lock_log_user_program_add(EVENT_SOURCE_KEYPAD,PROGRAM_EVENT_PIN_CODE_ADDED,user_ble_sn);    //添加普通密码
                        // kds_lockOpera_confirm_05(get_send_ten(),EVENT_TYPE_PROGRAM,EVENT_SOURCE_KEYPAD,PROGRAM_EVENT_PIN_CODE_ADDED,user_ble_sn,get_last_log_timestamp());
                        result = EVENT_RESULT_SUCCESS;
                    }
                    break;
                case CODE_HANDLE_CHANGE_MASTER:
                    modifyUserMasterCode(keyBoardEvent.input[0].buffer, keyBoardEvent.input[0].len);
                    // lock_log_user_program_add(EVENT_SOURCE_KEYPAD,PROGRAM_EVENT_MASTER_CODE_CHANGED,0xFE);      //修改管理员密码
                    // kds_lockOpera_confirm_05(get_send_ten(),EVENT_TYPE_PROGRAM,EVENT_SOURCE_KEYPAD,PROGRAM_EVENT_MASTER_CODE_CHANGED,0xFE,get_last_log_timestamp());
                    result = EVENT_RESULT_SUCCESS;
                    break;
                default:
                    break;
                }
            }
        }
        else
        {
            result = EVENT_RESULT_FAIL_DIFFERENT;
            OB_LOGE(TAG, "Fail: input is't same");
        }
        CLEAR_KEY_EVENT();
    }
    else
    {
        if ((isValidUserCode(keyBoardEvent.input[0].buffer, keyBoardEvent.input[0].len, &user_sn, true, false, false)) && (0 != user_sn))
        {
#if (Enabled == PRINTF_PASSWORD)
            OB_LOGD(TAG, "user sn[%u]: len[%u]", user_sn, keyBoardEvent.input[0].len);
#endif
            if ((CODE_HANDLE_ADD == handle_code) || (CODE_HANDLE_CHANGE_MASTER == handle_code))
            {
                result = EVENT_RESULT_FAIL_REPEAT;
            }
            else if (CODE_HANDLE_DEL == handle_code)
            {
                if (delUserCode(keyBoardEvent.input[0].buffer, keyBoardEvent.input[0].len, &user_sn))
                {
                    result = EVENT_RESULT_SUCCESS;
                }
                else
                {
                    result = EVENT_RESULT_DELETION_FAIL;
                }
            }
            CLEAR_KEY_EVENT();
        }
        else if ((CODE_HANDLE_ADD == handle_code) || (CODE_HANDLE_CHANGE_MASTER == handle_code))
        {
            result = EVENT_RESULT_SUCCESS;
            COPY_KEY_EVENT_INPUT();
        }
        else if (CODE_HANDLE_DEL == handle_code)
        {
            result = EVENT_RESULT_DELETION_FAIL;
            CLEAR_KEY_EVENT();
        }
        else
        {
            CLEAR_KEY_EVENT();
        }
    }

    baseEventPush(Q_HANDLE_SIG, result);
}

void keyEventHandleCode(uint8_t key_value, uint8_t handle_code, uint8_t input_cnt)
{
    if (KEY_CAN == key_value)
    {
        if (keyBoardEvent.input[0].len)
        {
            CLEAR_KEY_EVENT();
#if (Enabled == PRINTF_USER)
            OB_LOGD(TAG, "InputCode: input[%u]->", keyBoardEvent.input[0].len);
            OB_LOGD_DUMP(&keyBoardEvent.input[0].buffer[0], keyBoardEvent.input[0].len);
#endif
        }
        else
        {
            baseEventPush(Q_HANDLE_SIG, EVENT_RESULT_FAIL);
            CLEAR_KEY_EVENT();
        }
    }
    else if (KEY_OK == key_value)
    {
#if (Enabled == PRINTF_USER)
        if (keyBoardEvent.input[0].len)
        {
            OB_LOGD(TAG, "InputCode: input[%u]->", keyBoardEvent.input[0].len);
            OB_LOGD_DUMP(&keyBoardEvent.input[0].buffer[0], keyBoardEvent.input[0].len);
        }
#endif
        if (keyBoardEvent.input[0].len < USER_CODE_LEN_MIN)
        {
#if (Enabled == PRINTF_PASSWORD)
            OB_LOGE(TAG, "handle fail: input len is too short[%u]", keyBoardEvent.input[0].len);
#endif
            baseEventPush(Q_HANDLE_SIG, EVENT_RESULT_FAIL_INPUT);
            CLEAR_KEY_EVENT();
        }
        else
        {
            if (isTooSimpleCode(keyBoardEvent.input[0].buffer, keyBoardEvent.input[0].len))
            {
                CLEAR_KEY_EVENT();

#if (Enabled == PRINTF_PASSWORD)
                OB_LOGE(TAG, "add fail: code is too simple");
#endif
                baseEventPush(Q_HANDLE_SIG, EVENT_RESULT_FAIL_TOO_SIMPLE);
            }
            else
            {
                codeHandle(handle_code, input_cnt);
            }
        }
    }
    else if ((key_value < KEY_CNT) && (key_value != KEY_NUM_13))
    {
        if (keyBoardEvent.input[0].len < USER_CODE_LEN_MAX)
        {
            keyBoardEvent.input[0].buffer[keyBoardEvent.input[0].len] = key_value;
            keyBoardEvent.input[0].len++;
#if (Enabled == PRINTF_USER)
            if (keyBoardEvent.input[0].len)
            {
                OB_LOGD(TAG, "InputCode: input[%u]->", keyBoardEvent.input[0].len);
                OB_LOGD_DUMP(&keyBoardEvent.input[0].buffer[0], keyBoardEvent.input[0].len);
            }
#endif
        }
        else
        {
            // handle fail
#if (Enabled == PRINTF_PASSWORD)
            OB_LOGE(TAG, "handle fail: input fail");
#endif
            baseEventPush(Q_HANDLE_SIG, EVENT_RESULT_FAIL_INPUT);
            CLEAR_KEY_EVENT();
        }
    }
}

void keyEventLinkedUnlock(uint8_t key_value, uint8_t handle_code, uint8_t input_cnt)
{
    if (KEY_CAN == key_value)
    {
#if (Enabled == PRINTF_USER)
        OB_LOGD(TAG, "InputCode: input[%u]->", keyBoardEvent.input[0].len);
#endif
        if (keyBoardEvent.input[0].len)
        {
            CLEAR_KEY_EVENT();
        }
        else
        {
            baseEventPush(Q_HANDLE_SIG, EVENT_RESULT_FAIL);
            CLEAR_KEY_EVENT();
        }
    }
    else if (KEY_OK == key_value)
    {
#if (Enabled == PRINTF_USER)
        if (keyBoardEvent.input[0].len)
        {
            OB_LOGD(TAG, "InputCode: input[%u]->", keyBoardEvent.input[0].len);
            OB_LOGD_DUMP(&keyBoardEvent.input[0].buffer[0], keyBoardEvent.input[0].len);
        }
#endif
        if (keyBoardEvent.input[0].len < LINKED_CODE_LEN_MAX)
        {
#if (Enabled == PRINTF_PASSWORD)
            OB_LOGE(TAG, "handle fail: input len is too short[%u]", keyBoardEvent.input[0].len);
#endif
            baseEventPush(Q_HANDLE_SIG, EVENT_RESULT_FAIL_INPUT);
            CLEAR_KEY_EVENT();
        }
        else
        {
#if (Enabled == PRINTF_PASSWORD)
            OB_LOGE(TAG, "link unlock ok");
#endif
            linkKeyEventPush(&keyBoardEvent.input[0].buffer[0], keyBoardEvent.input[0].len);
        }
    }
    else if ((key_value < KEY_CNT) && (key_value != KEY_NUM_13))
    {
        if (keyBoardEvent.input[0].len < LINKED_CODE_LEN_MAX)
        {
            keyBoardEvent.input[0].buffer[keyBoardEvent.input[0].len] = key_value;
            keyBoardEvent.input[0].len++;
#if (Enabled == PRINTF_USER)
            if (keyBoardEvent.input[0].len)
            {
                OB_LOGD(TAG, "InputCode: input[%u]->", keyBoardEvent.input[0].len);
                OB_LOGD_DUMP(&keyBoardEvent.input[0].buffer[0], keyBoardEvent.input[0].len);
            }
#endif
        }
        else
        {
            // handle fail
#if (Enabled == PRINTF_PASSWORD)
            OB_LOGE(TAG, "handle fail: input fail");
#endif
            baseEventPush(Q_HANDLE_SIG, EVENT_RESULT_FAIL_INPUT);
            CLEAR_KEY_EVENT();
        }
    }
}

void keyEventAgingTest(uint8_t key_value, uint8_t input_cnt)
{
    if (KEY_CAN == key_value)
    {
        if (keyBoardEvent.input[0].len)
        {
            CLEAR_KEY_EVENT();
        }
        else
        {
            baseEventPush(Q_HANDLE_SIG, EVENT_RESULT_FAIL);
            CLEAR_KEY_EVENT();
        }
    }
    else if (KEY_OK == key_value)
    {
        if (keyBoardEvent.input[0].len < input_cnt)
        {
            baseEventPush(Q_HANDLE_SIG, EVENT_RESULT_FAIL_TOO_SHORT);
            CLEAR_KEY_EVENT();
        }
        else
        {
            uint32_t value = 0;
            value = arraysConvertNumber(keyBoardEvent.input[0].buffer, keyBoardEvent.input[0].len);

            if (1 == value)
            {
#if (Enabled == PRINTF_FSM)
                OB_LOGE(TAG, "----------------set aging time 2h----------------");
#endif
                handleEventPush(EVENT_RESULT_AGING_TEST_TIME,value);
            }
            else if (2 == value)
            {
#if (Enabled == PRINTF_FSM)
                OB_LOGE(TAG, "----------------set aging time 4h----------------");
#endif
                handleEventPush(EVENT_RESULT_AGING_TEST_TIME,value);
            }
            else if (3 == value)
            {
#if (Enabled == PRINTF_FSM)
                OB_LOGE(TAG, "----------------set aging time 8h----------------");
#endif
                handleEventPush(EVENT_RESULT_AGING_TEST_TIME,value);
            }
            else if (4 == value)
            { 
#if (Enabled == PRINTF_FSM)
                OB_LOGE(TAG, "----------------set aging time infinite----------------");
#endif
                handleEventPush(EVENT_RESULT_AGING_TEST_TIME,value);
            }
            CLEAR_KEY_EVENT();
        }
    }
    else if (key_value < KEY_CNT)
    {
        if (keyBoardEvent.input[0].len < input_cnt)
        {
            keyBoardEvent.input[0].buffer[keyBoardEvent.input[0].len] = key_value;
            keyBoardEvent.input[0].len++;
        }
        else
        {
            baseEventPush(Q_HANDLE_SIG, EVENT_RESULT_FAIL_TOO_LONG);
            CLEAR_KEY_EVENT();
        }
    }
}

