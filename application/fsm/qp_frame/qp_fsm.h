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
    uint8_t verify_fail_cnt;            // 验证错误次数
    uint32_t verify_fail_time;          // 验证错误的时间戳 
    uint8_t admin_flag;                 // 进入管理员菜单

}LockFsm;

/***************Function***************/
void qpFsmLoop(void);
void qpFsmInit(void);

/**************************************/

#endif 
