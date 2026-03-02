#include "task_qp_fsm.h"

#define OB_LOG_LEVEL OB_LOG_LEVEL_NONE
#include "ob_log.h"
#define TAG "task_qp_fsm"

/***************Variable***************/
static qp_fsm_task_driver_t qp_fsm_task_driver;

// ------------------------------------------

void qp_fsm_task_init(void)
{
    OB_LOGD(TAG, "Init[%s]", "fsm_task");

    qp_fsm_task_driver.io = bsp_qp_fsm_get_driver();
    if (NULL != qp_fsm_task_driver.io)
    {
        qp_fsm_task_driver.io->init();
    }
    else
    {
        OB_LOGE(TAG, "Error[%s]", "qp_fsm_task_driver.io is NULL");
    }
}

void qp_fsm_task_loop(void)
{
    qp_fsm_task_driver.io->loop();
}
