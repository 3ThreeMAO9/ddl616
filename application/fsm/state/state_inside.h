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
QState lock_fsm_sleep(LockFsm *me, QEvent const *e);
QState lock_fsm_idle(LockFsm *me, QEvent const *e);

QState lockFsmWake(LockFsm *me, QEvent const *e);
QState lockFsmSystemLock(LockFsm *me, QEvent const *e);
QState lockFsmLowEnergySystemLock(LockFsm *me, QEvent const *e);
QState lockFsmHandleSuccessKeepGreen(LockFsm *me, QEvent const *e);
QState lockFsmVerifyAdminSuccess(LockFsm *me, QEvent const *e);
QState lockFsmHandleSuccess(LockFsm *me, QEvent const *e);
QState lockFsmVerifyUserSuccess(LockFsm *me, QEvent const *e);
QState lockFsmUnlockSuccess(LockFsm *me, QEvent const *e);
QState lockFsmLockSuccess(LockFsm *me, QEvent const *e);
QState lockFsmHandleFailKeepRed(LockFsm *me, QEvent const *e);
QState lockFsmHandleFail(LockFsm *me, QEvent const *e);
QState lockFsmVerifyFail(LockFsm *me, QEvent const *e);
QState lockFsmTimeOut(LockFsm *me, QEvent const *e);
QState lockFsmAdminMenu(LockFsm *me, QEvent const *e);
QState lockFsmVacationMode(LockFsm *me, QEvent const *e);
QState lockFsmSetAutoLockTimeDelay(LockFsm *me, QEvent const *e);
QState lockFsmAutoLock(LockFsm *me, QEvent const *e);
QState lockFsmSilentMode(LockFsm *me, QEvent const *e);
QState lockFsmChangeMasterCode(LockFsm *me, QEvent const *e);
QState lockFsmAddUserCode(LockFsm *me, QEvent const *e);
QState lockFsmDeleteUserCode(LockFsm *me, QEvent const *e);
QState lockFsmAddUserFingers(LockFsm *me, QEvent const *e);
QState lockFsmDeleteUserFingers(LockFsm *me, QEvent const *e);
QState lockFsmAddOneTimeCode(LockFsm *me, QEvent const *e);
QState lockFsmProduceTest(LockFsm *me, QEvent const *e);
QState lockFsmAgingTest(LockFsm *me, QEvent const *e);
QState lockFsmDeviceTest(LockFsm *me, QEvent const *e);
QState lockFsmInputError(LockFsm *me, QEvent const *e);
/**************************************/


/*****************Macro****************/

#endif 
