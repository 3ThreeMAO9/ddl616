#include "qp_fsm.h"
#include "event_queue.h"

#define OB_LOG_LEVEL OB_LOG_LEVEL_NONE
#include "ob_log.h"
#define TAG "qp_fsm"

/***************Variable***************/
static LockFsm lock_fsm;

// ------------------------------------------

static void lockFsm_ctor(LockFsm* lock_fsm)
{
    QFsm_ctor(&(lock_fsm->super), (QStateHandler)(lock_fsm_init));
    lock_fsm->branch = NULL;
    lock_fsm->verify_fail_cnt = 0;
    lock_fsm->verify_fail_time = 0;
    lock_fsm->admin_flag = false;
}

void qpFsmInit(void)
{
    QEvent event;

    eventQueueInit();     // event' queue init

    lockFsm_ctor(&lock_fsm);

    event.sig = Q_INIT_SIG;
    QFsmInit((QFsm *)(&lock_fsm), (QEvent*)(&event));
}

void qpFsmLoop(void)
{
    QEvent event;

    if (true == eventQueuePop(&event))
    {
        QFsm_dispatch((QFsm *)(&lock_fsm), (QEvent*)(&event));
    }
}
