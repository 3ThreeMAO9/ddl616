/**
 * @file task_player.h
 * @author Dandjinh Deng (Dandjinh_Deng@on-bright.com)
 * @brief 
 * @version 0.1
 * @date 2023-02-23
 * 
 * @copyright Copyright (c) 2023 广州昂宝电子有限公司
 * 
 */

#ifndef _PLAYER_TASK
#define _PLAYER_TASK

#include <stdint.h>
#include "task_player_def.h"

#define INPUT_END_CHAR 0xFFFF

enum voice_id_e_def
{
    VOICE_ID_INVALID_INPUT,
    VOICE_ID_TODO,
    VOICE_ID_MAX
};
typedef enum voice_id_e_def voice_id_e;

void player_task_init(void);

void player_task_loop(void);

uint32_t player_task_play_list_clear(void);

void player_task_list_add(int num_1, ...);

void player_task_language_set(uint8_t language);

void player_task_volume_set(int8_t level);

void operate_tips_set(uint8_t enable);

uint32_t play_task_is_busy(void);

uint32_t player_task_list_size_get(void);

uint32_t player_task_register_callback(module_player_callback_t callback);

void play_num(uint32_t num);

void voice_play_hex_bytes(const uint8_t* data, uint8_t len);

#define PLAYER_LIST_ADD(num, ...)                                                                                                              \
    do {                                                                                                                                       \
        player_task_list_add(num, ##__VA_ARGS__, INPUT_END_CHAR);                                                                              \
    } while(0)

#define PLAYER_LIST_CLEAR_ADD(num, ...)                                                                                                        \
    do {                                                                                                                                       \
        player_task_play_list_clear();                                                                                                         \
        PLAYER_LIST_ADD(num, ##__VA_ARGS__);                                                                                                   \
    } while(0)

///强制播报
#define PLAYER_LIST_ADD_FORCE(num, ...)                                                                                                        \
    do {                                                                                                                                       \
        operate_tips_set(1);                                                                                                                   \
        PLAYER_LIST_ADD(num, ##__VA_ARGS__);                                                                                                   \
        operate_tips_set(0);                                                                                                                   \
    } while(0)

///强制播报
#define PLAYER_LIST_CLEAR_ADD_FORCE(num, ...)                                                                                                  \
    do {                                                                                                                                       \
        operate_tips_set(1);                                                                                                                   \
        player_task_play_list_clear();                                                                                                         \
        PLAYER_LIST_ADD(num, ##__VA_ARGS__);                                                                                                   \
        operate_tips_set(0);                                                                                                                   \
    } while(0)

#endif // _PLAYER_TASK
