#ifndef FLASH_DATA__HH
#define FLASH_DATA__HH

#include "config.h"
#include "item_config.h"
#include "user.h"
#include "bsp_rom_config.h"

/*****************Macro****************/
#define BATCH_SIZE                      (512)

/*****************Enum*****************/
typedef enum {
    FLASH_DATA_VALID = 0x01,      // 页数据有效
    FLASH_DATA_INVALID = 0xFF,    // 页数据无效/空
} flash_data_status_t;

/****************Struct****************/
#pragma pack(1)


#pragma pack()




/***************Variable***************/


/***************Function***************/
void flash_data_init(void);
void flash_page_system_init(void);
void save_user_data(uint16_t user_sn, uint8_t* pData);
void save_parameter_data(uint8_t* pData, uint16_t size);
void save_wakeupstate_data(uint8_t* pData, uint16_t size);

// ========== 固定地址用户读写接口 ==========

// 密码用户
uint8_t read_code_user(uint8_t index, user_info_t* pData);
void save_code_user(uint8_t index, user_info_t* pData);

// 指纹用户
uint8_t read_finger_user(uint8_t index, user_info_t* pData);
void save_finger_user(uint8_t index, user_info_t* pData);

// 卡片用户
uint8_t read_card_user(uint8_t index, user_info_t* pData);
void save_card_user(uint8_t index, user_info_t* pData);

// 人脸用户
uint8_t read_face_user(uint8_t index, user_info_t* pData);
void save_face_user(uint8_t index, user_info_t* pData);

// ========== 通用接口 ==========
void save_user_data(uint16_t user_sn, uint8_t* pData);
uint8_t read_user_data_with_check(uint16_t user_sn, user_info_t* pData);
void flash_update_user_cnt(user_key_cnt_t* cnt);

// ========== 工具函数 ==========
uint32_t get_user_addr_by_sn(uint16_t user_sn);

// ========== 用户档案读写 ==========
uint8_t read_profile(uint8_t index, user_profile_t* profile);
void    save_profile(uint8_t index, user_profile_t* profile);
uint8_t find_profile_idx_by_user_id(uint8_t user_id);
uint8_t read_profile_by_user_id(uint8_t user_id, user_profile_t* profile);
void    del_profile(uint8_t user_id);
uint8_t get_profile_cnt(void);


#endif /* FLASH_DATA__HH */
