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

#include <stdint.h>
#include "max44009.h"
#include "shuttest.h"
#include "i2c.h"
#include "stm32f1xx_hal.h"


#define MAX44009_ADDR (0x94)
//0x94
//(0x7B)

#define MAX44009_INTR_STATUS    0
#define MAX44009_INTR_ENABLE    1
#define MAX44009_CONFIG         2

#define MAX44009_LUX_HIGHT      3
#define MAX44009_LUX_LOW        4

#define MAX44009_TRESHOLD_UP    5
#define MAX44009_TRESHOLD_LOW   6
#define MAX44009_TRESHOLD_TME   7

#define MAX44009_CONFIG_CONTINIUS_MODE (1<<7)
#define MAX44009_CONFIG_MANUAL_MODE    (1<<6)
//#define MAX44009_CONFIG_TIMER_MIN      ((1 << 0) | (1 << 1) | (1 << 2))
#define MAX44009_CONFIG_TIMER_MIN      ((1 << 0) | (1 << 1))

#define MAX44009_INT_GPIO_CLK_ENABLE()     __HAL_RCC_GPIOA_CLK_ENABLE()
#define MAX44009_INT_GPIO_CLK_DISABLE()    __HAL_RCC_GPIOA_CLK_DISABLE()
#define MAX44009_INT_EXTI_IRQn             EXTI0_IRQn

#define MAX44009_UP_TRESHOLD     2
#define MAX44009_DOWN_TRESHOLD   2

static void max44009_config_gpio_int();
/**
 * @brief         initialization
 * @details       configure GPIO A.3 for input mode
 *                this port connected to light sensor inrrupt line
 */
void max44009_init() {
    max44009_config_gpio_int();
    i2c_write_byte(MAX44009_ADDR, MAX44009_CONFIG,
                   MAX44009_CONFIG_CONTINIUS_MODE |
                   MAX44009_CONFIG_MANUAL_MODE |
                   MAX44009_CONFIG_TIMER_MIN);
    i2c_write_byte(MAX44009_ADDR, MAX44009_TRESHOLD_TME, 0);
}

/**
 * @brief         Read sensor lux
 *
 * @return        return vlue in format
 *                  e3 e2 e1 e0 m7 m6 m5 m4 m3 m2 m1 m0
 *                 where e - bits for exponent
 *                       m - bits for mantissa
 *                 or -1 for not sucess
 */
int16_t max44009_get_dev_lux(max44009_dev_lux *lux) {
#if 0
    uint8_t low ;
    uint8_t hi;
    int rc;

    // Lux = 2^(EXP) * Mantissa * 0.045
    // EXP = HIGHT[7:4]
    // Mantissa = HIGHT[3:0] LOW[3:0]
    rc = i2c_read_byte(MAX44009_ADDR, MAX44009_LUX_HIGHT);
    if(rc < 0)
        return -1;
    hi = rc & 0xff;
    rc = i2c_read_byte(MAX44009_ADDR, MAX44009_LUX_LOW);
    if(rc < 0)
        return -1;
    low = rc & 0xff;
    lux->mantissa = (uint8_t)(((((uint16_t)hi)<<4) | (low & 0xf)) & 0xff);
    lux->exponent = (uint8_t)(((hi)>>4) & 0xf);
    return 0;
#else
    uint8_t hi;
    int rc;

    // Lux = 2^(EXP) * Mantissa * 0.72
    // EXP = HIGHT[7:4]
    // Mantissa = HIGHT[3:0] LOW[3:0]
    rc = i2c_read_byte(MAX44009_ADDR, MAX44009_LUX_HIGHT);
    if(rc < 0)
        return -1;
    hi = rc & 0xff;
    lux->mantissa = (uint8_t)(hi & 0xf);
    lux->exponent = (uint8_t)(((hi)>>4) & 0xf);
    return 0;

#endif
}

int16_t max44009_debug_get_all_regs(uint8_t regs[]) {
    uint8_t reg;
    int rc;
    for(reg = 0; reg <= MAX44009_MAX_REG_NUM; reg++) {
        rc = i2c_read_byte(MAX44009_ADDR, reg);
        if(rc < 0) {
            return rc;
        }
        regs[reg] = (uint8_t)rc;
    }
    return reg;
}

int16_t max44009_enable_irq() {
    return i2c_write_byte(MAX44009_ADDR, MAX44009_INTR_ENABLE, 1);
}

int16_t max44009_disable_irq() {
    return i2c_write_byte(MAX44009_ADDR, MAX44009_INTR_ENABLE, 0);
}

int16_t max44009_clear_irq_flag() {
    int rc;

    rc = i2c_read_byte(MAX44009_ADDR, MAX44009_INTR_STATUS);
    if(rc < 0)
        return -1;

    return 0;
}

int8_t max44009_set_up_treshold() {
    uint8_t current;
    int rc;

    rc = i2c_read_byte(MAX44009_ADDR, MAX44009_LUX_HIGHT);
    if(rc < 0)
        return -1;
    current = rc & 0xff;


    rc = i2c_write_byte(MAX44009_ADDR, MAX44009_TRESHOLD_UP,
                        current+MAX44009_UP_TRESHOLD);
    if(rc < 0)
        return 0;

    rc = i2c_write_byte(MAX44009_ADDR, MAX44009_TRESHOLD_LOW, 1);
    if(rc < 0)
        return 0;
    return current;
}

void max44009_set_down_treshold(uint8_t value) {
    int rc;

    rc = i2c_write_byte(MAX44009_ADDR, MAX44009_TRESHOLD_LOW,
                        value+MAX44009_DOWN_TRESHOLD);
    if(rc < 0)
        return;
    i2c_write_byte(MAX44009_ADDR, MAX44009_TRESHOLD_UP, 0xee);

}


static void max44009_config_gpio_int() {
    GPIO_InitTypeDef gpioinitstruct = {0};

    /* Enable the corresponding Push Button clock */
    MAX44009_INT_GPIO_CLK_ENABLE();

    /* Configure Push Button pin as input */
    gpioinitstruct.Pin    = MAX44009_INT_PIN;
    gpioinitstruct.Pull   = GPIO_NOPULL;
    gpioinitstruct.Speed  = GPIO_SPEED_FREQ_HIGH;


    gpioinitstruct.Mode = GPIO_MODE_IT_FALLING;
    HAL_GPIO_Init(MAX44009_INT_PORT, &gpioinitstruct);

    /* Enable and set EXTI Interrupt to the fighest priority */
    HAL_NVIC_SetPriority((IRQn_Type)(MAX44009_INT_EXTI_IRQn), 0x0E, 0);
    HAL_NVIC_EnableIRQ((IRQn_Type)(MAX44009_INT_EXTI_IRQn));

}
