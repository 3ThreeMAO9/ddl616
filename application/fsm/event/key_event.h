#ifndef KEY_EVENT__HH
#define KEY_EVENT__HH

#include "config.h"


/*****************Macro****************/


/*****************Enum*****************/
typedef enum{
    CODE_HANDLE_ADD = 0,
    CODE_HANDLE_CHANGE_MASTER,
    CODE_HANDLE_DEL,

}code_handle_t;

/****************Struct****************/


/***************Variable***************/
typedef struct
{
    uint8_t len;
    uint8_t buffer[DUMMY_USER_CODE_LEN_MAX];

}key_board_input_t;

typedef struct{
    key_board_input_t input[2];

}key_board_event_t;

/***************Function***************/
void keyEventInit(void);
void keyEventVerifyAdmin(uint8_t key_value);
void keyEventVerifyUser(uint8_t key_value);
void keyEventSetParameter(uint8_t key_value, uint8_t parameter_index, uint8_t width);
void keyEventHandleCode(uint8_t key_value, uint8_t handle_code, uint8_t input_cnt);
void keyEventAgingTest(uint8_t key_value, uint8_t input_cnt);
/**************************************/

#endif 
