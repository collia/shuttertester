/**
 * @file    shuttest.c
 * @author  Nikolay
 * @license MIT
 * @date    2020-07-07
 * @brief   calculation time between interrupts
 *
 * 
 */

#define ST_MAX_TESTS_NUMBER 10

void st_init();
void st_start();
uint32_t st_stop();
void st_irq();
