#include "qp_port.h" /* the port of the QEP event processor */
#include "type_def.h"       // Enabled / Disabled（下面 FSM_DISPATCH_LOG 的开关）
#include "fsm_state.h"      // lock_fsm_init
#include "state_inside.h"   // 其余状态处理函数的声明（下面名字表要用）

#define OB_LOG_LEVEL OB_LOG_LEVEL_DEFAULT
#include "ob_log.h"
#define TAG "fsm"

/***************Variable***************/
static QEvent const QEP_reservedEvt_[]={
    {(QSignal)(Q_EMPTY_SIG), (uint8_t)0},
    {(QSignal)(Q_ENTRY_SIG), (uint8_t)0},
    {(QSignal)(Q_EXIT_SIG), (uint8_t)0},
    {(QSignal)(Q_INIT_SIG), (uint8_t)0},
};

// ===== 状态调试打印：统一在引擎这一处，状态函数里不再各写一行 =====
// C 里函数指针取不出名字，所以维护一张"状态处理函数 -> 名字"表；名字用 #fn 自动取，不会写错。
// 新增状态时在这里补一行即可（漏了只会打印 "?"，不影响功能）
#if (Enabled == PRINTF_FSM)

#define FSM_STATE_NAME(fn)          { (QStateHandler)(fn), #fn }

static const struct
{
    QStateHandler handler;
    const char *name;
} fsm_state_names_[] = {
    // 初始 / 主状态
    FSM_STATE_NAME(lock_fsm_init),
    FSM_STATE_NAME(lock_fsm_power_on),
    FSM_STATE_NAME(lock_fsm_idle),
    FSM_STATE_NAME(lock_fsm_sleep),
    FSM_STATE_NAME(lock_fsm_verify_admin),
    FSM_STATE_NAME(lock_fsm_reset),
    FSM_STATE_NAME(lock_fsm_system_lock),
    FSM_STATE_NAME(lock_fsm_low_power_system_lock),
    FSM_STATE_NAME(lock_fsm_aging_test),
    // 菜单状态
    FSM_STATE_NAME(lock_fsm_menu_admin),
    FSM_STATE_NAME(lock_fsm_menu_modfiy_admin_pin),
    FSM_STATE_NAME(lock_fsm_menu_system_settings),
    FSM_STATE_NAME(lock_fsm_menu_create_linked_unlock),
    FSM_STATE_NAME(lock_fsm_menu_join_linked_unlock),
    FSM_STATE_NAME(lock_fsm_menu_add_normal_user_settings),
    FSM_STATE_NAME(lock_fsm_menu_add_normal_pw),
    FSM_STATE_NAME(lock_fsm_menu_add_normal_nfc),
    FSM_STATE_NAME(lock_fsm_menu_add_normal_finger),
    FSM_STATE_NAME(lock_fsm_menu_modify_admin_user_settings),
    FSM_STATE_NAME(lock_fsm_menu_delete_normal_user),
    FSM_STATE_NAME(lock_fsm_menu_delete_normal_user_input_id),
    FSM_STATE_NAME(lock_fsm_menu_delete_all_normal_user),
    // 结果状态
    FSM_STATE_NAME(lockFsmHandleAddSuccess),
    FSM_STATE_NAME(lockFsmDeleteUserSuccess),
    FSM_STATE_NAME(lockFsmDeleteUserFailAdmin),
    FSM_STATE_NAME(lockFsmDeleteUserFailNotExist),
    FSM_STATE_NAME(lockFsmDeleteUserFailEmpty),
    FSM_STATE_NAME(lockFsmDeleteUserFailTimeOut),
    FSM_STATE_NAME(lockFsmHandleWakeUpSuccess),
    FSM_STATE_NAME(lockFsmHandleVoiceModeSuccess),
    FSM_STATE_NAME(lockFsmHandleSuccess),
    FSM_STATE_NAME(lockFsmVerifyUserSuccess),
    FSM_STATE_NAME(lockFsmHandleFail),
    FSM_STATE_NAME(lockFsmHandleCardRepeat),
    FSM_STATE_NAME(lockFsmHandleAddFail),
    FSM_STATE_NAME(lockFsmVerifyFail),
    FSM_STATE_NAME(lockFsmInputError),
};

// 查表：找不到就返回 "?"（新状态忘了登记时）
static const char *fsm_state_name_of(QStateHandler handler)
{
    uint8_t i;

    for (i = 0; i < (uint8_t)(sizeof(fsm_state_names_) / sizeof(fsm_state_names_[0])); i++)
    {
        if (fsm_state_names_[i].handler == handler)
        {
            return fsm_state_names_[i].name;
        }
    }
    return "?";
}

// 打印"哪个状态在收哪个事件"；关掉打印（PRINTF_FSM = Disabled）时整句不参与编译
#define FSM_DISPATCH_LOG(handler, evt)                                              \
    OB_LOGD(TAG, "Fsm_state[%s], Event[%u, %u]",                                    \
            fsm_state_name_of(handler), (evt)->sig, (evt)->dynamic_[0])

#else   // 关掉打印：表、查表、调用全部不编译

#define FSM_DISPATCH_LOG(handler, evt)

#endif

// ------------------------------------------
void QFsmInit(QFsm *me, QEvent const *e)
{
    //  执行QFsm超状态的状态处理函数，就是init
    FSM_DISPATCH_LOG(me->state, e);
    (*me->state)(me, e); /* execute the top-most initial transition */

    //  进入目的状态，手动指定状态切换事件(用信号Q_ENTRY_SIG指定)，并处理状态切换事件
    //  QEP内部维护一个不变的保留事件数组 QEP_reservedEvt_[ ]。用于保存信号对应的事件
    FSM_DISPATCH_LOG(me->state, &QEP_reservedEvt_[Q_ENTRY_SIG]);
    (void)(*me->state)(me, &QEP_reservedEvt_[Q_ENTRY_SIG]);/* enter the target */
}

//  事件生成函数
void QFsm_dispatch(QFsm *me, QEvent const *e)
{
    //  在栈空间中临时保存，防止执行事件处理函数切换状态后丢失源状态
    QStateHandler s = me->state; /* save the current state */
    //  调用当前状态中对应的事件处理函数
    FSM_DISPATCH_LOG(s, e);
    QState r = (*s)(me, e);      /* call the event handler */
    if (r == Q_RET_TRAN) //  执行事件处理函数后发生了状态转换
    {                                                           /* transition taken? */
        //  退出源状态，调用源状态的事件处理函数（发送信号Q_EXIT_SIG）
        FSM_DISPATCH_LOG(s, &QEP_reservedEvt_[Q_EXIT_SIG]);
        (void)(*s)(me, &QEP_reservedEvt_[Q_EXIT_SIG]);          /* exit the source */
        //  进入目的状态，调用目的状态的事件处理函数（发送信号Q_ENTRY_SIG）
        FSM_DISPATCH_LOG(me->state, &QEP_reservedEvt_[Q_ENTRY_SIG]);
        (void)(*me->state)(me, &QEP_reservedEvt_[Q_ENTRY_SIG]); /*enter target*/
    }
    else if (r == Q_RET_HANDLED)
    {
        FSM_DISPATCH_LOG(me->state, &QEP_reservedEvt_[Q_INIT_SIG]);
        (void)(*me->state)(me, &QEP_reservedEvt_[Q_INIT_SIG]); /*enter target*/
    }
}
