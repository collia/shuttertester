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
#include <string.h>

#include "shuttest.h"
#include "max44009.h"
#include "board.h"
#include "time.h"

static bool st_mode_wait_hight_level = true;
static uint8_t low_value = 0;

static uint32_t tests[ST_MAX_TESTS_NUMBER];
static uint32_t test_index;
static uint64_t start_time;
void st_init() {
}

void st_start() {
    memset(tests, 0, sizeof(tests));
    start_time = 0;
    test_index = 0;
    st_mode_wait_hight_level = true;   
    low_value = max44009_set_up_treshold();
    max44009_enable_irq();
}
uint32_t st_stop() {
    uint32_t result = test_index;
    max44009_disable_irq();
    return result;
}

uint32_t st_get_result(const uint32_t **result) {
    *result = tests; 
    return test_index;
}

void st_irq() {
    if(st_mode_wait_hight_level) {
        start_time = TIME_get();
        BRD_led_on();
        max44009_set_down_treshold(low_value);
        max44009_clear_irq_flag();
        st_mode_wait_hight_level = false;
    } else {
        if(test_index < ST_MAX_TESTS_NUMBER) {
            tests[test_index] = (uint32_t)(TIME_get() - start_time);
            test_index++;
        }
        BRD_led_off();
        low_value = max44009_set_up_treshold();
        max44009_clear_irq_flag();
        st_mode_wait_hight_level = true;
    }
}
