/**
 * @file    max44009.c
 * @author  Nikolay
 * @license MIT
 * @date    2020-07-05
 * @brief   Driver for light sensor MAX 44009
 *
 * Driver works with light sensor connected by i2c
 * with address 0x94.
 * external interrup connected to a.3, but it is not used now
 */

#ifndef _MAX44009_H_
#define  _MAX44009_H_
#include <stdint.h>

typedef struct {
    uint8_t mantissa;
    uint8_t exponent;
} max44009_dev_lux;

#define MAX44009_INT_PORT    GPIOA
#define MAX44009_INT_PIN     GPIO_PIN_0             /* PA.00*/


#define MAX44009_MAX_REG_NUM 7

void max44009_init();
int16_t max44009_get_dev_lux(max44009_dev_lux *lux);
int16_t max44009_debug_get_all_regs(uint8_t regs[]);
int16_t max44009_enable_irq();
int16_t max44009_disable_irq();
int16_t max44009_clear_irq_flag();
int8_t max44009_set_up_treshold();
void max44009_set_down_treshold(uint8_t value);
#endif
