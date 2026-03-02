#include "qp_port.h" /* the port of the QEP event processor */

#define OB_LOG_LEVEL OB_LOG_LEVEL_NONE
#include "ob_log.h"
#define TAG "qp_port"

/***************Variable***************/
static QEvent const QEP_reservedEvt_[]={
    {(QSignal)(Q_EMPTY_SIG), (uint8_t)0},
    {(QSignal)(Q_ENTRY_SIG), (uint8_t)0},
    {(QSignal)(Q_EXIT_SIG), (uint8_t)0},
    {(QSignal)(Q_INIT_SIG), (uint8_t)0},
};

// ------------------------------------------
void QFsmInit(QFsm *me, QEvent const *e)
{
    //  执行QFsm超状态的状态处理函数，就是init
  
    (*me->state)(me, e); /* execute the top-most initial transition */

    //  进入目的状态，手动指定状态切换事件(用信号Q_ENTRY_SIG指定)，并处理状态切换事件
    //  QEP内部维护一个不变的保留事件数组 QEP_reservedEvt_[ ]。用于保存信号对应的事件
    (void)(*me->state)(me, &QEP_reservedEvt_[Q_ENTRY_SIG]);/* enter the target */
}

//  事件生成函数
void QFsm_dispatch(QFsm *me, QEvent const *e)
{
    //  在栈空间中临时保存，防止执行事件处理函数切换状态后丢失源状态
    QStateHandler s = me->state; /* save the current state */
    //  调用当前状态中对应的事件处理函数
    QState r = (*s)(me, e);      /* call the event handler */
    if (r == Q_RET_TRAN) //  执行事件处理函数后发生了状态转换
    {                                                           /* transition taken? */
        //  退出源状态，调用源状态的事件处理函数（发送信号Q_EXIT_SIG）
        (void)(*s)(me, &QEP_reservedEvt_[Q_EXIT_SIG]);          /* exit the source */
        //  进入目的状态，调用目的状态的事件处理函数（发送信号Q_ENTRY_SIG）
        (void)(*me->state)(me, &QEP_reservedEvt_[Q_ENTRY_SIG]); /*enter target*/
    }
    else if (r == Q_RET_HANDLED)
    {
        (void)(*me->state)(me, &QEP_reservedEvt_[Q_INIT_SIG]); /*enter target*/
    }
}
