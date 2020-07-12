
/**
File contains constants for evaluation board http://wiki.stm32duino.com/index.php?title=Blue_Pill

*/
#include "stm32f1xx_hal.h"

/* LED */

#define LED1_PIN                         GPIO_PIN_13             /* PC.13*/
#define LED1_GPIO_PORT                   GPIOC
#define LED1_GPIO_CLK_ENABLE()           __HAL_RCC_GPIOC_CLK_ENABLE()
#define LED1_GPIO_CLK_DISABLE()          __HAL_RCC_GPIOC_CLK_DISABLE()
 

void BRD_led_init(void);
void BRD_led_toggle(void);
void BRD_led_on(void);
void BRD_led_off(void);
