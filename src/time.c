/**
 * @file    time.c<shuttertester>
 * @author  Nikolay
 * @license MIT
 * @date    2020-07-26
 * @brief   File contains implementation time module based on timer
 */

#include "time.h"

#include <stdint.h>

#include "stm32f1xx_hal.h"
#include "board.h"
#include "main.h"



static uint64_t time_ms;
static TIM_HandleTypeDef  TimHandle;

void TIME_init() {

    __HAL_RCC_TIM2_CLK_ENABLE();

    /* ##- Configure the NVIC for TIMx ######################################## */
    /* Set Interrupt Group Priority */
    HAL_NVIC_SetPriority(TIM2_IRQn, 5, 0);

    /* Enable the TIMx global Interrupt */
    HAL_NVIC_EnableIRQ(TIM2_IRQn);

     /*##-1- Configure the TIM peripheral */

    /* Set TIMx instance */
    TimHandle.Instance = TIME_TIM;
    /* Initialize TIMx peripheral for period 0.1 ms
    */

    TimHandle.Init.Period            = 10 - 1;
    TimHandle.Init.Prescaler         = SystemCoreClock/100000 - 1;
    TimHandle.Init.ClockDivision     = 0;
    TimHandle.Init.CounterMode       = TIM_COUNTERMODE_UP;
    TimHandle.Init.RepetitionCounter = 0;
    TimHandle.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;

    /*
    TimHandle.Init.Period            = 1000 - 1;
    TimHandle.Init.Prescaler         = SystemCoreClock/1000000-1;//SystemCoreClock/100000 - 1;
    TimHandle.Init.ClockDivision     = 0;
    TimHandle.Init.CounterMode       = TIM_COUNTERMODE_UP;
    TimHandle.Init.RepetitionCounter = 0;
    TimHandle.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
    */
    if (HAL_TIM_Base_Init(&TimHandle) != HAL_OK)
    {
        /* Initialization Error */
        Error_Handler();
    }

    /*##-2- Start the TIM Base generation in interrupt mode ####################*/
    /* Start Channel1 */
    if (HAL_TIM_Base_Start_IT(&TimHandle) != HAL_OK)
    {
        /* Starting Error */
        Error_Handler();
    }
}

uint64_t TIME_get() {
    uint64_t time = 0;
    //HAL_NVIC_DisableIRQ(TIM2_IRQn);
    //time = time_ms;
    //HAL_NVIC_EnableIRQ(TIM2_IRQn);
    return time;
}



/**
  * @brief  Period elapsed callback in non blocking mode
  * @param  htim : TIM handle
  * @retval None
  */
void TIME_PeriodElapsedCallback(TIM_HandleTypeDef *htim){
    if(TIME_TIM == htim->Instance) {
        time_ms++;
        //BRD_led_toggle();
    }
}


void TIME_IRQHandler()
{
    HAL_TIM_IRQHandler(&TimHandle);
}
