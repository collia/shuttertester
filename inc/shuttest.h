/**
 * @file    shuttest.c
 * @author  Nikolay
 * @license MIT
 * @date    2020-07-07
 * @brief   calculation time between interrupts
 *
 * 
 */

#define ST_MAX_TESTS_NUMBER 20

void st_init();
void st_start();
uint32_t st_stop();
uint32_t st_get_result(const uint32_t **result);
void st_irq();
