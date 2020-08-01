/**
 * @file    shuttest.c
 * @author  Nikolay
 * @license MIT
 * @date    2020-07-07
 * @brief   calculation time between interrupts
 *
 * 
 */

#include <stdbool.h>
#include <stdint.h>

#include "shuttest.h"
#include "max44009.h"
#include "board.h"
#include "time.h"

static bool st_mode_wait_hight_level = true;
static uint8_t low_value = 0;

void st_init() {
}

void st_start() {
    st_mode_wait_hight_level = true;   
    low_value = max44009_set_up_treshold();
    max44009_enable_irq();
}
uint32_t st_stop() {
    uint32_t result = 0;
    max44009_disable_irq();
    return result;
}

void st_irq() {
    if(st_mode_wait_hight_level) {
        //BRD_led_on();
        max44009_set_down_treshold(low_value);
        max44009_clear_irq_flag();
        st_mode_wait_hight_level = false;
    } else {
        //BRD_led_off();
        low_value = max44009_set_up_treshold();
        max44009_clear_irq_flag();
        st_mode_wait_hight_level = true;
    }
}
