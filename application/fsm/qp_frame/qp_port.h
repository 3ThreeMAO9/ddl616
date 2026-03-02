#ifndef QP_PORT__HH
#define QP_PORT__HH

#include "config.h"

#define EVENT_DYNAMIC_SIZE                          (8)

typedef uint8_t QSignal;

//  事件处理对事件处理的状态
typedef uint8_t QState;          /* status returned from a state-handler function */
//  状态处理函数指针，本设计中状态处理函数就表示状态，有typedef表示指定它的类型为QState，
//  相当于一种声明，而非定义产生实例

/****************Struct****************/

typedef struct QEvnetTag{
    uint8_t sig;
    uint8_t dynamic_[EVENT_DYNAMIC_SIZE];
}QEvent;

/* pointer to function type definition */
typedef QState (*QStateHandler)(void *me, QEvent const *e);
    /* return type */
    /* name of the pointer-to-function type */
    /* argument list */ //  一个通用状态机的指针和一个 QEvent指针
typedef struct QFsmTag
{                        /* Finite State Machine */
    //  当前处于的状态，指向状态处理函数
    QStateHandler state; /* current active state */
} QFsm; //  派生各个状态机结构的基本类

/*****************Macro****************/
#define QFsm_ctor(me_, initial_) ((me_)->state = (initial_))

//  从状态处理函数到事件处理器的返回状态
#define Q_RET_HANDLED ((QState)0)
#define Q_RET_IGNORED ((QState)1)
#define Q_RET_TRAN ((QState)2)

//  一个状态处理函数，每当它处理了当前的事件时，返回宏 Q_HANDLED( ) 。
#define Q_HANDLED() (Q_RET_HANDLED)
//  一个状态处理函数，每当它忽略（不处理）当前的事件时，返回宏 Q_IGNORED( )
#define Q_IGNORED() (Q_RET_IGNORED)
//  逗号表达式表示执行逗号前语句，但整个表达式的值为逗号后变量，优先级比'='更低，
//  先执行((QFsm *)me)->state = (QStateHandler)(target_)，但Q_TRAN(target_)值为Q_RET_TRAN
//  这里可以用(QFsm *)强制转换me是因为派生类me的第一个成员变量就是它的父类QFsm实例，内存起始位置和me一样
#define Q_TRAN(target_) (((QFsm *)me)->state = (QStateHandler)(target_), Q_RET_TRAN)

/*****************Enum*****************/
//  内部使用的信号
//  QEP内部维护一个不变的保留事件数组 QEP_reservedEvt_[ ]。用于保存信号对应的事件
enum QReservedSignals
{
    Q_EMPTY_SIG = 0,    /* signal for coding empty actions */
    Q_ENTRY_SIG,        /* signal for coding entry actions */
    Q_EXIT_SIG,         /* signal for coding exit actions */
    Q_INIT_SIG,         /* signal for coding initial transitions */
    Q_USER_SIG          /* first signal that can be used in user applications */
};

/***************Variable***************/


/***************Function***************/
void QFsmInit(QFsm *me, QEvent const *e);
void QFsm_dispatch(QFsm *me, QEvent const *e);

/**************************************/

#endif 
