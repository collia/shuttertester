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
#include "adc_dma.h"

#ifdef USE_MAX4409

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

#else
typedef struct {
    uint32_t grow_up_start;
    uint32_t fail_down_end;
} test_points;

#define DMA_BUFFER_LENGTH 1024*3
#define RESULT_POINTS_NUMBER 20

#define POINTS_TO_MS_10(point) ((point)*2)

static volatile bool adc_started = false;
static volatile uint16_t *adc_finished_buffer = NULL;
static uint16_t  buffer_1[DMA_BUFFER_LENGTH];
static uint16_t  buffer_2[DMA_BUFFER_LENGTH];

static test_points points[RESULT_POINTS_NUMBER];
static uint32_t current_point;
static uint32_t samples_processed;
static uint32_t result[RESULT_POINTS_NUMBER];

static uint32_t minimal_value = 0xffff;

static void st_adc_stop_callback();
static void st_extract_data(volatile const uint16_t *adc_data, uint32_t start_sample);

void st_init() {
    adc_dma_set_stop_callback(st_adc_stop_callback);
}

void st_start() {
    memset(buffer_1, 0, sizeof(buffer_1));
    memset(buffer_2, 0, sizeof(buffer_2));
    adc_dma_start(buffer_1,
                  buffer_2,
                  sizeof(buffer_1));
    adc_started = true;
    current_point = 0;
    samples_processed = 0;
    memset(points, 0, sizeof(points));
    minimal_value = 0xffff;
}

void st_stop() {
    adc_dma_stop();
    adc_started = false;
}

uint32_t st_process() {
    if(adc_finished_buffer) {
        st_extract_data(adc_finished_buffer, samples_processed);
        samples_processed += DMA_BUFFER_LENGTH;
        adc_finished_buffer = NULL;
        return samples_processed;
    }
    return 0;
}

uint32_t st_get_result(const uint32_t **r) {
    int i;
    int result_number = 0;
    for(i=0; i< current_point; i++) {
        if((points[i].fail_down_end && points[i].grow_up_start) &&
           (points[i].fail_down_end >= points[i].grow_up_start)) {
            result[result_number] = POINTS_TO_MS_10(points[i].fail_down_end - points[i].grow_up_start);
            result_number++;
        }
    }
    *r = result;
    return result_number;
}

const void* st_get_debug_data() {
    return points;
    //return buffer_1;
}

static void st_adc_stop_callback(uint16_t* data) {
    //TERM_debug_print("Stop\n\r");
    adc_finished_buffer = data;
}

static uint32_t st_moving_average_init(const uint16_t *data, uint32_t depth) {
    uint32_t res = 0;
    for(int i = 0; i < depth; i++) {
        res += data[i];
    }
    return res;
}

static uint32_t st_moving_average_update(uint32_t old_ma,
                                         const uint16_t* data,
                                         uint32_t idx,
                                         uint32_t depth) {
    uint32_t old_sum = old_ma;
    return (old_sum - data[idx-depth] + data[idx]);
}

static void st_extract_data(volatile const uint16_t *adc_data, uint32_t start_sample) {
    int i = 0;
    static const uint32_t moving_average_depth = 32;
    uint32_t ma_old = st_moving_average_init(adc_data, moving_average_depth);
    uint32_t ma = st_moving_average_update(ma_old,
                                           adc_data,
                                           moving_average_depth,
                                           moving_average_depth);
    
    
    for(i = moving_average_depth+1; i < DMA_BUFFER_LENGTH; i++) {
        if(ma < minimal_value) {
            minimal_value = ma;
            TERM_debug_print("min: ");
            TERM_debug_print_int(ma);
            TERM_debug_print("\r\n");
        }
        if ((ma > minimal_value * 2+50) &&
            (ma_old < minimal_value * 2+50)) {
            points[current_point].grow_up_start = start_sample + i;
            TERM_debug_print("ma: ");
            TERM_debug_print_int(ma);
            TERM_debug_print(" ma_old: ");
            TERM_debug_print_int(ma_old);
            TERM_debug_print("\r\n");

        }
        if ((ma < (minimal_value * 2-50)) &&
            (ma_old > (minimal_value * 2- 50))) {
            TERM_debug_print("ma_old: ");
            TERM_debug_print_int(ma_old);
            TERM_debug_print(" ma: ");
            TERM_debug_print_int(ma);
            TERM_debug_print("\r\n");

            points[current_point].fail_down_end = start_sample + i;
            if(current_point < RESULT_POINTS_NUMBER-1) {
                current_point++;
            }
        }
        ma_old = ma;
        ma = st_moving_average_update(ma_old,
                                      adc_data,
                                      i,
                                      moving_average_depth);
        
    }
    //TERM_debug_print_int(current_point);
    //TERM_debug_print("\r\n\r\n");
}
#endif
