
/**
File contains controls for evaluation board http://wiki.stm32duino.com/index.php?title=Blue_Pill

*/

#include "board.h"

void BRD_led_init()
{
    GPIO_InitTypeDef  GPIO_InitStruct;
    LED1_GPIO_CLK_ENABLE();

    /* -2- Configure IO in output push-pull mode to drive external LEDs */
    GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull  = GPIO_PULLDOWN;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;

    GPIO_InitStruct.Pin = LED1_PIN;
    HAL_GPIO_Init(LED1_GPIO_PORT, &GPIO_InitStruct);
}




void BRD_led_toggle()
{
    HAL_GPIO_TogglePin(LED1_GPIO_PORT, LED1_PIN);
}

void BRD_led_on()
{
    HAL_GPIO_WritePin(LED1_GPIO_PORT, LED1_PIN, GPIO_PIN_RESET);
}

void BRD_led_off()
{
    HAL_GPIO_WritePin(LED1_GPIO_PORT, LED1_PIN, GPIO_PIN_SET);
}
