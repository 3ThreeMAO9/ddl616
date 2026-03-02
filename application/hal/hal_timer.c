#include "hal_timer.h"
#include "timer16.h"
#include <math.h>

#define TAG "hal_timer"

static hal_timer_callback_t hal_timer_callback = NULL;

void hal_callback(unsigned char ucChannel)
{
    hal_timer_callback();
}

void hal_timer_register_callback(hal_timer_callback_t cb)
{
    hal_timer_callback = cb;
}

uint32_t hal_timer_start(hal_timer_config_t const *p_config)
{
    uint32_t interval = 0;

    interval = p_config->period;

    hal_timer_register_callback(p_config->event_handler);
    // -------------------NVIC enable-------------------
    NVIC_SetPriority(CT16B0_IRQn, 1);
    NVIC_EnableIRQ(CT16B0_IRQn);
    // -------------------------------------------------
    TIMER16_Delay_Interrupt(OB_CT16B0, TIMER16_MR1, (TIMER16_MR_I | TIMER16_MR_R), interval);
    return 0;
}

uint32_t hal_timer_stop(HalTimerSel timer_id)
{
    OB_CT16B_Type *time_dev = (timer_id == HAL_TIMER0) ? OB_CT16B0 : OB_CT16B1;
    IRQn_Type irq_num = (timer_id == HAL_TIMER0) ? CT16B0_IRQn : CT16B1_IRQn;

    TIMER16_Close(time_dev);
    NVIC_DisableIRQ(irq_num);
    return 0;
}

void time0_irq_handler(void)
{
    // 执行用户回调
    if (hal_timer_callback) {
        hal_timer_callback();
    }
}

