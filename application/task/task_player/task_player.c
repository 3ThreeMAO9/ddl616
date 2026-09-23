/**
 * @file task_player.c
 * @author Dandjinh Deng (Dandjinh_Deng@on-bright.com)
 * @brief 
 * @version 0.1
 * @date 2023-03-01
 * 
 * @copyright Copyright (c) 2023 广州昂宝电子有限公司
 * 
 */

#include "task_player.h"
#include "task_player_def.h"
#include "module_player.h"
#include "ob_error_codes.h"
// #include "system_parameter.h"
// #include "shell.h"
#include "bsp_system_def.h"
#include <stdarg.h>
#include "parameter.h"
#include "system_timer.h"
#include "event.h"

#define OB_LOG_LEVEL OB_LOG_LEVEL_NONE
#include "ob_log.h"
#define TAG "player"

static player_task_handle_t     player_task_handle = {0};
static module_player_callback_t module_player_callback = NULL;


static void this_player_task_callback(const player_event_t *event)
{
    OB_LOGD(TAG,"%s   error_code[%ld], event[%ld]",__FUNCTION__,event->error_code,event->event);
    if ((event->error_code == OB_SUCCESS) && (event->event == EVENT_PLAY_END))
    {
        baseEventPush(Q_HANDLE_SIG, EVENT_RESULT_VOICE_TIME_OUT);
    }
    if(module_player_callback!=NULL)
        module_player_callback(event);
}

void player_task_init(void)
{
    player_task_handle.driver = get_bsp_player_driver();
    player_task_handle.driver->init();

    player_task_volume_set(OB_LOCK_VOLUME_HIGH);        //设置音量

    uint8_t data = 0;
    data = readUserParameter(USER_PARA_LANGUAGE_MODE_ID);
    switch (data)
    {
    case LANGUAGE_CN:
        player_task_language_set(OB_LOCK_LANGUAGE_CN); //设置语言
        break;
    case LANGUAGE_EN:
        player_task_language_set(OB_LOCK_LANGUAGE_EN); //设置语言
        break;
    case LANGUAGE_SP:
        player_task_language_set(OB_LOCK_LANGUAGE_SP); //设置语言
        break;
    case LANGUAGE_FR:
        player_task_language_set(OB_LOCK_LANGUAGE_FR); //设置语言
        break;
    default:
        break;
    }

    player_task_handle.driver->register_callback(this_player_task_callback);

    OB_LOGD(TAG, "init");
}

void player_task_loop(void)
{
    player_task_handle.driver->loop();
}


uint32_t player_task_play_list_clear()
{
    player_task_handle.driver->player_list_clear();
    return 0;
}

/// @brief 
/// @param level  VOLUME_HIGH ,VOLUME_MIDDLE,VOLUME_LOW
void player_task_volume_set(int8_t level)
{
   if((level <= OB_LOCK_VOLUME_MAX) && (level >= OB_LOCK_VOLUME_MIN)) {
       player_task_handle.driver->volume_level_set(level);
   }
}

void player_list_handler(uint16_t value)
{
    OB_LOGD(TAG,"value:%d\r\n",value);
    if(value < SOUND_START) {
        player_task_handle.driver->player_list_add(player_task_handle.language, value);
    }
    else if((value < SOUND_END) && (value > SOUND_START)) {
        player_task_handle.driver->player_list_add(SOUND_EFFECT, value - SOUND_START - 1);
    }
}

void player_task_list_add(int num_1, ...)
{
    va_list p_args;
    va_start(p_args, num_1);
    int idx = 0;
    int val = num_1;
    while(val != INPUT_END_CHAR){
        ++idx;
        player_list_handler(val);
        val = va_arg(p_args, int); //得到下个变参值
    }
    va_end(p_args);
}

void player_task_language_set(uint8_t language)
{
    player_task_handle.language = language;
}

/// @brief 注册回调函数，NULL可取消回调
/// @param callback 
/// @return 
uint32_t player_task_register_callback(module_player_callback_t callback)
{
    module_player_callback = callback;
    
    return OB_SUCCESS;
}

uint32_t sys_volume_get(void)
{
    return 3;
}

void operate_tips_set(uint8_t enable)
{
    static uint32_t cnt = 0;
    if(sys_volume_get() == OB_LOCK_VOLUME_MUTE) {
        if(enable == 1) {
            cnt++;
            player_task_volume_set(OB_LOCK_VOLUME_HIGH);
        }
        else {
            if(cnt > 0) {
                cnt--;
            }
            if(cnt == 0)
                player_task_volume_set(OB_LOCK_VOLUME_MUTE);
        }
    }
}

uint32_t player_task_list_size_get(void)
{
    return player_task_handle.driver->list_size_get();
}

uint32_t play_task_is_busy(void)
{
    return player_task_handle.driver->is_busy();
}

static uint16_t voice_get_id(uint8_t c)
{
    if (c <= 9) {
        return VOICE_Zero + c;
    } else if (c <= 35) {
        return SOUND_A + (c - 10);
    }
    return 0;   // 无效
}
static void voice_play_one_char(uint8_t c)
{
    uint16_t id = voice_get_id(c);
    if (id != 0)
        PLAYER_LIST_ADD(id);
}

void voice_play_hex_bytes(const uint8_t* data, uint8_t len)
{
    if (data == NULL || len == 0) 
        return;

    player_task_play_list_clear();

    for (uint8_t i = 0; i < len; i++) {
        voice_play_one_char((data[i] >> 4) & 0x0F);
        voice_play_one_char(data[i] & 0x0F);
    }
}

void play_num(uint32_t num)
{
    uint8_t temp;
    uint8_t t = 0;
    uint8_t voice_data[10];
    player_task_play_list_clear();
    if (!num)
    {
        PLAYER_LIST_ADD(VOICE_Zero);
        return;
    }
    memset(voice_data, 0, sizeof(voice_data));
    while (num)
    {
        voice_data[++t] = VOICE_Zero + num % 10;
        num /= 10;
    }
    for (temp = 0; temp < t; temp++)
    {
        PLAYER_LIST_ADD(voice_data[t - temp]);
    }
}

//测试
#if 1
// player_task_volume(VOLUME_HIGH);
// player_task_language_set(OB_LOCK_LANGUAGE_CN);

void player_task_callback(const player_event_t *event)
{
    if(event->event == EVENT_PLAY_END)
    {
        OB_LOGD(TAG,"%s\r\n",__FUNCTION__);
    }
}

void test_voice(uint16_t value,uint16_t cnt)
{
    for (uint16_t i = 0; i < cnt; i++)
        PLAYER_LIST_ADD(value + i);
}

void test_china(uint16_t value,uint16_t cnt)
{
    player_task_language_set(OB_LOCK_LANGUAGE_CN);
    for (uint16_t i = 0; i < cnt; i++)
        PLAYER_LIST_ADD(value + i);
}

void test_english(uint16_t value,uint16_t cnt)
{
    player_task_language_set(OB_LOCK_LANGUAGE_EN);
    for (uint16_t i = 0; i < cnt; i++)
        PLAYER_LIST_ADD(value + i);
}

void test_spanish(uint16_t value,uint16_t cnt)
{
    player_task_language_set(OB_LOCK_LANGUAGE_SP);
    for (uint16_t i = 0; i < cnt; i++)
        PLAYER_LIST_ADD(value + i);
}

void test_french(uint16_t value,uint16_t cnt)
{
    player_task_language_set(OB_LOCK_LANGUAGE_FR);
    for (uint16_t i = 0; i < cnt; i++)
        PLAYER_LIST_ADD(value + i);
}

void test_sound(uint16_t value, uint16_t cnt)
{
    player_task_register_callback(player_task_callback);
    for (uint16_t i = 0; i < cnt; i++)
        PLAYER_LIST_ADD(SOUND_START + value + i);
}

void set_language_en(void)
{
    player_task_language_set(OB_LOCK_LANGUAGE_EN);      //设置语言
}

void set_language_sp(void)
{
    player_task_language_set(OB_LOCK_LANGUAGE_SP);      //设置语言
}

void set_language_fr(void)
{
    player_task_language_set(OB_LOCK_LANGUAGE_FR);      //设置语言
}

#endif







