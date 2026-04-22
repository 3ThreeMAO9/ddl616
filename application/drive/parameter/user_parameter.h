#ifndef USER_PARAMETER__HH
#define USER_PARAMETER__HH

#include "config.h"


typedef uint32_t U_PARA;

typedef uint8_t (*user_parameter_flash_callback_t)(uint16_t, uint8_t*, uint16_t);

/*****************Macro****************/
#define DEVICE_SN_LEN_MAX                       32


/*****************Enum*****************/
typedef enum
{
    PARAMETER_TAMPER_ALARM          = 0,    ///< 防撬报警
    PARAMETER_LOITER_ALARM          = 1,    ///< 逗留报警
    PARAMETER_FACE_FUNC_SETTING     = 2,    ///< 人脸功能设置
    PARAMETER_HUMAN_SENSOR_SETTING  = 3,    ///< 人体感应设置
    PARAMETER_ID_CNT,                       ///< 有效参数总数
} parameter_id_e;
typedef enum{
    DEVICE_SN_OB_ID = 0,
    
    DEVICE_SN_CNT,

}device_sn_id_t;

/****************Struct****************/
#pragma pack(1)

typedef struct{
    U_PARA min_value;
    U_PARA max_value;
    U_PARA default_value;

}parameter_range_t;

typedef struct {
    uint8_t flag;
    uint8_t lenth;
    uint8_t sn[32];

} finger_chip_t;

typedef struct {
    uint8_t flag;
    uint8_t sensitivity[16];
} touch_chip_t;

typedef struct{
    U_PARA sum;

    U_PARA value[PARAMETER_ID_CNT];

    finger_chip_t finger_chip;              // 指纹唯一码
    touch_chip_t touch_chip;                // 触摸灵敏度

}user_patameter_tab_t;
#pragma pack()

/***************Variable***************/


/***************Function***************/
uint8_t user_parameter_init(void);
uint32_t get_user_parameter(uint8_t index);
uint8_t set_user_parameter(uint8_t index, const uint32_t value);

uint8_t write_finger_module_chip_sn(const uint8_t* chip_sn, uint8_t lenth);
uint8_t read_finger_module_chip_sn(uint8_t* chip_sn);

uint8_t write_touch_chip_sensitivity(const uint8_t* sensitivity);
uint8_t read_touch_chip_sensitivity(uint8_t* sensitivity);

/**************************************/

#endif 
