/**
 * @file    time.h<shuttertester>
 * @author  Nikolay
 * @license MIT
 * @date    2020-07-26
 * @brief   File contains implementation time module based on timer
 */

#include <stdint.h>
#include "stm32f1xx_hal.h"


#define TIME_TIM TIM2

void TIME_init();
uint64_t TIME_get();
void TIME_PeriodElapsedCallback(TIM_HandleTypeDef * htim);
void TIME_IRQHandler();
