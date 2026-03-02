#ifndef QP_FSM__HH
#define QP_FSM__HH

#include "config.h"
#include "qp_port.h"

/*****************Macro****************/


/*****************Enum*****************/


/****************Struct****************/
#pragma pack(1)


#pragma pack()


/***************Variable***************/
//  继承自QFsm的状态机，增加了自定义的一些参数
typedef struct{
    QFsm super;                         /* derive from QFsm */
    
    QStateHandler branch;               /* branch of state */

    // uint8_t pin_verify_count;           // 验证错误计数
    // uint8_t card_verify_count;          // 验证错误计数
}LockFsm;

/***************Function***************/
void qpFsmLoop(void);
void qpFsmInit(void);

/**************************************/

#endif 
