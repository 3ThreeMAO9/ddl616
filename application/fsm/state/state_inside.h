#ifndef STATE_INSIDE__HH
#define STATE_INSIDE__HH

#include "config.h"

#include "event.h"
#include "qp_port.h"
#include "qp_fsm.h"

/*****************Macro****************/

/*****************Enum*****************/

/****************Struct****************/

/***************Variable***************/

/***************Function***************/
// 状态切换的调试打印已统一到引擎一处：见 qp_frame/qp_port.c 的 FSM_DISPATCH_LOG
// （各状态/菜单处理函数里不再写打印）
QState lock_fsm_sleep(LockFsm *me, QEvent const *e);
QState lock_fsm_idle(LockFsm *me, QEvent const *e);
QState lock_fsm_power_on(LockFsm *me, QEvent const *e);
QState lock_fsm_verify_admin(LockFsm *me, QEvent const *e);
QState lock_fsm_reset(LockFsm *me, QEvent const *e);
QState lock_fsm_low_power_system_lock(LockFsm *me, QEvent const *e);

QState lock_fsm_menu_admin(LockFsm *me, QEvent const *e);
QState lock_fsm_menu_modfiy_admin_pin(LockFsm *me, QEvent const *e);
QState lock_fsm_menu_repeat_input_code(LockFsm *me, QEvent const *e, uint8_t code_handle);
QState lock_fsm_menu_system_settings(LockFsm *me, QEvent const *e);
QState lock_fsm_menu_add_normal_pw(LockFsm *me, QEvent const *e);
QState lock_fsm_menu_create_linked_unlock(LockFsm *me, QEvent const *e);
QState lock_fsm_menu_join_linked_unlock(LockFsm *me, QEvent const *e);
QState lock_fsm_menu_add_normal_user_settings(LockFsm *me, QEvent const *e);
QState lock_fsm_menu_add_normal_pw(LockFsm *me, QEvent const *e);
QState lock_fsm_menu_add_normal_nfc(LockFsm *me, QEvent const *e);
QState lock_fsm_menu_add_normal_finger(LockFsm *me, QEvent const *e);
QState lock_fsm_menu_modify_admin_user_settings(LockFsm *me, QEvent const *e);
QState lock_fsm_menu_delete_normal_user(LockFsm *me, QEvent const *e);
QState lock_fsm_menu_delete_normal_user_input_id(LockFsm *me, QEvent const *e);
QState lock_fsm_menu_delete_all_normal_user(LockFsm *me, QEvent const *e);

QState lock_fsm_aging_test(LockFsm *me, QEvent const *e);
QState lockFsmHandleWakeUpSuccess(LockFsm *me, QEvent const *e);
QState lock_fsm_system_lock(LockFsm *me, QEvent const *e);
QState lockFsmHandleVoiceModeSuccess(LockFsm *me, QEvent const *e);
QState lockFsmHandleAddSuccess(LockFsm *me, QEvent const *e);
QState lockFsmDeleteUserSuccess(LockFsm *me, QEvent const *e);
QState lockFsmDeleteUserFailAdmin(LockFsm *me, QEvent const *e);
QState lockFsmDeleteUserFailNotExist(LockFsm *me, QEvent const *e);
QState lockFsmDeleteUserFailEmpty(LockFsm *me, QEvent const *e);
QState lockFsmDeleteUserFailTimeOut(LockFsm *me, QEvent const *e);
QState lockFsmHandleSuccess(LockFsm *me, QEvent const *e);
QState lockFsmVerifyUserSuccess(LockFsm *me, QEvent const *e);
QState lockFsmHandleFail(LockFsm *me, QEvent const *e);
QState lockFsmHandleCardRepeat(LockFsm *me, QEvent const *e);
QState lockFsmHandleAddFail(LockFsm *me, QEvent const *e);
QState lockFsmVerifyFail(LockFsm *me, QEvent const *e);
QState lockFsmInputError(LockFsm *me, QEvent const *e);
/**************************************/

/*****************Macro****************/

#endif 
