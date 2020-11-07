/**
 * @file    shuttest.c
 * @author  Nikolay
 * @license MIT
 * @date    2020-07-07
 * @brief   calculation time between interrupts
 *
 *
 */
#ifdef USE_MAX4409
#define ST_MAX_TESTS_NUMBER 20

void st_init();
void st_start();
uint32_t st_stop();
uint32_t st_get_result(const uint32_t **result);
void st_irq();
#else
#define ST_ADC_CHANNELS_NUMBER 2
void st_init();
void st_start();
void st_stop();
uint32_t st_get_result(const uint32_t **r, uint32_t channel);
uint32_t st_process(void);
const void* st_get_debug_data();
#endif
