/**
 * @file    main.c<shuttertester>
 * @author  Nikolay
 * @license MIT
 * @date    2020-07-07
 * @brief   File contains main munction and initialization
 *
 *
 */
#include <stdbool.h>

#include "stm32f1xx_hal.h"
#include "usbd_core.h"
#include "stm32f1xx_hal_pcd.h"
#include "usbd_desc.h"
#include "usbd_cdc.h"
#include "usbd_cdc_interface.h"

#include "shuttest_terminal.h"
#include "shuttest.h"
#include "board.h"
#include "max44009.h"
#include "terminal.h"
#include "i2c.h"
#include "main.h"
#include "time.h"


USBD_HandleTypeDef USBD_Device;

void SystemClock_Config(void);

/**
 * @brief         Main function
 */
int main(void)
{
  /* Reset of all peripherals, Initializes the Flash interface and the Systick.
   */
  HAL_Init();

  /* Configure the system clock to 72 MHz */
  SystemClock_Config();


  BRD_led_init();
  BRD_led_off();

  /* Init Device Library */
  USBD_Init(&USBD_Device, &VCP_Desc, 0);

  /* Add Supported Class */
  USBD_RegisterClass(&USBD_Device, USBD_CDC_CLASS);

  /* Add CDC Interface Class */
  USBD_CDC_RegisterInterface(&USBD_Device, &USBD_CDC_fops);

  /* Start Device Process */
  USBD_Start(&USBD_Device);

  i2c_master_init();
  max44009_init();
  st_init();
  TIME_init();

  /* Infinite loop */
  while (1)
  {
      SHTEST_term_parser();
      //TERM_debug_print("I'm alive\r\n");
      //HAL_Delay(10000);
  }
}

void SystemClock_Config(void)
{
  RCC_ClkInitTypeDef clkinitstruct = { 0 };
  RCC_OscInitTypeDef oscinitstruct = { 0 };
  RCC_PeriphCLKInitTypeDef rccperiphclkinit = { 0 };

  /* Enable HSE Oscillator and activate PLL with HSE as source */
  oscinitstruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  oscinitstruct.HSEState = RCC_HSE_ON;
  oscinitstruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  oscinitstruct.PLL.PLLMUL = RCC_PLL_MUL9;

  oscinitstruct.PLL.PLLState = RCC_PLL_ON;
  oscinitstruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;

  if (HAL_RCC_OscConfig(&oscinitstruct) != HAL_OK)
  {
    /* Start Conversation Error */
    Error_Handler();
  }

  /* USB clock selection */
  rccperiphclkinit.PeriphClockSelection = RCC_PERIPHCLK_USB;
  rccperiphclkinit.UsbClockSelection = RCC_USBCLKSOURCE_PLL_DIV1_5;
  HAL_RCCEx_PeriphCLKConfig(&rccperiphclkinit);

  /* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2
   * clocks dividers */
  clkinitstruct.ClockType = RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK |
                            RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;

  clkinitstruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  // todo HSE starts unstable
  //clkinitstruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSE;
  clkinitstruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  clkinitstruct.APB1CLKDivider = RCC_HCLK_DIV1;
  clkinitstruct.APB2CLKDivider = RCC_HCLK_DIV1;
  if (HAL_RCC_ClockConfig(&clkinitstruct, FLASH_LATENCY_2) != HAL_OK)
  {
    /* Start Conversation Error */
    Error_Handler();
  }
}

/**
  * @brief  This function is executed in case of error occurrence.
  */
void Error_Handler(void)
{
  /* Turn LED3 on */
  //BSP_LED_On(LED3);
  while (1)
  {
      BRD_led_toggle();
      /* Insert delay 100 ms */
      HAL_Delay(100);
  }
}

/**
  * @brief EXTI line detection callbacks
  * @param GPIO_Pin: Specifies the pins connected EXTI line
  * @retval None
  */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    if (GPIO_Pin == MAX44009_INT_PIN)
    {
        //TERM_debug_print("irq\r\n");
        st_irq();
    }
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim){
    if(TIME_TIM == htim->Instance) {
        TIME_PeriodElapsedCallback(htim);
    } else if(USBD_CDC_TIM == htim->Instance) {
        USBD_CDC_TIM_PeriodElapsedCallback(htim);
    }
}

#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t * file, uint32_t line)
{
  /* User can add his own implementation to report the file name and line
   * number, ex: printf("Wrong parameters value: file %s on line %d\r\n", file,
   * line) */

  /* Infinite loop */
  while (1)
  {
  }
}

#endif
