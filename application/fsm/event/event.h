#ifndef EVENT__HH
#define EVENT__HH

#include "config.h"
#include "qp_port.h"

/*****************Macro****************/

/*****************Enum*****************/
//  内部使用的信号
typedef enum
{ /* all signals for the Bomb FSM */
    Q_WORK_TIME_OUT_SIG = Q_USER_SIG,           //4
    Q_FUNCTION_TIME_OUT_SIG,
    Q_WARN_TIME_OUT_SIG,
    Q_KEY_BOARD_PRESS_SIG,
    Q_KEY_PRESS_SIG,
    Q_KEY_BOARD_RELEASE_SIG,
    Q_HANDLE_SIG,
    Q_USER_KEY_SIG,
    Q_NFC_KEY_SIG,
    
    Q_SIG_CNT,

    EVENT_SIG_NULL = 0xFF,

}LockSignals;

typedef enum{
    USER_EVENT_SUCCESS = 0,
    USER_EVENT_SUCCESS_VERIFY_ADMIN,
    USER_EVENT_SUCCESS_VERIFY_USER,
    
    USER_EVENT_SUCCESS_VERIFY_REGISTER,
    USER_EVENT_SUCCESS_VERIFY_UNLOCK,
    USER_EVENT_SUCCESS_VERIFY_REMOTE_UNLOCK,
    USER_EVENT_SUCCESS_VERIFY_SET_PARAMETER,
    USER_EVENT_SUCCESS_VERIFY_DELETE,

    USER_EVENT_SUCCESS_REGISTER,
    USER_EVENT_SUCCESS_MODIFY,
    USER_EVENT_SUCCESS_DELETE,
    
    USER_EVENT_FAIL,
    USER_EVENT_FAIL_INVALID,
    USER_EVENT_FAIL_INVALID_USER,
    USER_EVENT_FAIL_INVALID_ADMIN,
    USER_EVENT_FAIL_TOO_SHORT,
    USER_EVENT_FAIL_TOO_LONG,
    USER_EVENT_FAIL_FULL,
    USER_EVENT_FAIL_EMPTY,
    USER_EVENT_FAIL_SIMPTY,
    USER_EVENT_FAIL_REPEAT,

    USER_EVENT_CNT,

}user_key_event_e;

typedef enum{
    HANDLE_EVENT_SUCCESS = 0,
    HANDLE_EVENT_FAIL,
    HANDLE_EVENT_WAKE,
    HANDLE_EVENT_TAMPER_WARN,
    HANDLE_EVENT_UART_RX,
    HANDLE_EVENT_SLEEP,
    HANDLE_EVENT_SLEEP_BUSY,
    HANDLE_EVENT_CNT,
}handle_event_e;

/****************Struct****************/


/***************Variable***************/


/***************Function***************/
void baseEventPush(uint8_t event_type, uint8_t value);
void userKeyEventPush(uint8_t result, uint8_t type, uint16_t user_sn);
void handleEventPush(uint8_t handle, uint8_t value);
void nfcKeyEventPush(uint8_t* id, uint8_t size);

/**************************************/

#endif 
