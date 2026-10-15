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
QState lock_fsm_menu_modfiy_admin_finger(LockFsm *me, QEvent const *e);         // 添加/修改管理指纹
QState lock_fsm_menu_repeat_change_master_code(LockFsm *me, QEvent const *e);   // 再输一次管理密码
QState lock_fsm_menu_repeat_add_code(LockFsm *me, QEvent const *e);             // 再输一次普通密码

QState lock_fsm_aging_test(LockFsm *me, QEvent const *e);
QState lock_fsm_device_test(LockFsm *me, QEvent const *e);       // 整机测试（258#），第一步按键检测
QState lockFsmDeviceTestSuccess(LockFsm *me, QEvent const *e);     // 整机测试：播"测试成功"，播完回 me->branch
QState lockFsmDeviceTestFail(LockFsm *me, QEvent const *e);       // 整机测试：播"测试失败"，播完回 me->branch
QState lockFsmHandleWakeUpSuccess(LockFsm *me, QEvent const *e);
QState lock_fsm_system_lock(LockFsm *me, QEvent const *e);
QState lockFsmHandleVoiceModeSuccess(LockFsm *me, QEvent const *e);
QState lockFsmHandleAddSuccess(LockFsm *me, QEvent const *e);
QState lock_fsm_menu_delete_wait_done(LockFsm *me, QEvent const *e);   // 等指纹模板删完再报成功
QState lock_fsm_exit_admin_mode(LockFsm *me, QEvent const *e);         // 退出管理模式提示，播完进休眠
QState lockFsmMenuTimeOut(LockFsm *me, QEvent const *e);               // 子菜单操作超时提示，播完回上级菜单
QState lock_fsm_remote_add_finger(LockFsm *me, QEvent const *e);       // 云端(0x02)发起的指纹录入，录完回待机
QState lock_fsm_remote_add_card(LockFsm *me, QEvent const *e);         // 云端(0x02)发起的卡片录入，刷完回待机
QState lockFsmDeleteUserSuccess(LockFsm *me, QEvent const *e);
QState lockFsmDeleteUserFailAdmin(LockFsm *me, QEvent const *e);
QState lockFsmDeleteUserFailNotExist(LockFsm *me, QEvent const *e);
QState lockFsmDeleteUserFailEmpty(LockFsm *me, QEvent const *e);
QState lockFsmDeleteUserFailTimeOut(LockFsm *me, QEvent const *e);
QState lockFsmAddFailTimeOut(LockFsm *me, QEvent const *e);           // 添加类界面操作超时提示，播完回上级菜单
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
