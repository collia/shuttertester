/**
 * @file    adc_dma.c
 * @author  Nikolay
 * @license MIT
 * @date    2020-08-23
 * @brief   File conteins API for initialization and usage 
 *          ADC with DMA data transfer
 *
 * Used Timer3 for adc control
 */
#include "stm32f1xx_hal.h"
#include "stm32f1xx_hal_dma.h"
#include "stm32f1xx_hal_adc.h"

#include "adc_dma.h"

#define DMA_NUMBER_OF_BUFFERS 2

ADC_HandleTypeDef    AdcHandle;
adc_dma_stop_clbk    adc_stop_clbk = NULL;
#if 0
static void dma_init();
#endif
static void adc_init();
static void adc_dma_hal_init(ADC_HandleTypeDef *hadc);

static uint32_t* dma_data[DMA_NUMBER_OF_BUFFERS];
static uint32_t dma_data_in_use = 0;
static uint32_t dma_data_size;

void adc_dma_init() {
    adc_dma_hal_init(&AdcHandle);
    adc_init();

    /* Run the ADC calibration */
    if (HAL_ADCEx_Calibration_Start(&AdcHandle) != HAL_OK)
    {
        /* Calibration Error */
        Error_Handler();
    }

}

void adc_dma_start(uint16_t* data1, uint16_t* data2, uint32_t data_len) {
    dma_data[0] = (uint32_t*)data1;
    dma_data[1] = (uint32_t*)data2;
    dma_data_in_use = 0;
    dma_data_size = data_len / (sizeof(uint16_t));  // DMA configured for half-word transitions
    if (HAL_ADC_Start_DMA(&AdcHandle,
                          dma_data[dma_data_in_use],
                          dma_data_size) != HAL_OK){
        Error_Handler();
    }
}

void adc_dma_set_stop_callback(adc_dma_stop_clbk clbk) {
    adc_stop_clbk = clbk;
}

void adc_dma_stop() {
    if (HAL_ADC_Stop_DMA(&AdcHandle) != HAL_OK)
    {
        /* Start Error */
        Error_Handler();
    }
}

 
/**
  * @brief  Conversion complete callback in non blocking mode
  * @param  AdcHandle : AdcHandle handle
  * @note   This example shows a simple way to report end of conversion
  *         and get conversion result. You can add your own implementation.
  * @retval None
  */
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *AdcHandle)
{
    /* Report to main program that ADC sequencer has reached its end */
    if(adc_stop_clbk) {
        adc_stop_clbk((uint16_t*)dma_data[dma_data_in_use]);
    }
    if(++dma_data_in_use >= DMA_NUMBER_OF_BUFFERS) {
        dma_data_in_use = 0;
    }

    if (HAL_ADC_Start_DMA(AdcHandle,
                          dma_data[dma_data_in_use],
                          dma_data_size) != HAL_OK){
        Error_Handler();
    }
    BRD_led_toggle();
}

/**
  * @brief  Conversion DMA half-transfer callback in non blocking mode
  * @param  hadc: ADC handle
  * @retval None
  */
void HAL_ADC_ConvHalfCpltCallback(ADC_HandleTypeDef* hadc)
{

}

/**
  * @brief  ADC error callback in non blocking mode
  *        (ADC conversion with interruption or transfer by DMA)
  * @param  hadc: ADC handle
  * @retval None
  */
void HAL_ADC_ErrorCallback(ADC_HandleTypeDef *hadc)
{
  /* In case of ADC error, call main error handler */
  Error_Handler();
}

void adc_dma_adc_irq() {
    HAL_ADC_IRQHandler(&AdcHandle);
}

void adc_dma_dma_irq() {
    HAL_DMA_IRQHandler(AdcHandle.DMA_Handle);
}

static void adc_init() {
    ADC_ChannelConfTypeDef   sConfig;

    /* Configuration of ADCx init structure: ADC parameters and regular group */
    AdcHandle.Instance = ADC1;

    AdcHandle.Init.DataAlign             = ADC_DATAALIGN_RIGHT;
    AdcHandle.Init.ScanConvMode          = ADC_SCAN_ENABLE;
    AdcHandle.Init.ContinuousConvMode    = DISABLE;
    AdcHandle.Init.NbrOfConversion       = 1;
    AdcHandle.Init.DiscontinuousConvMode = DISABLE;
    AdcHandle.Init.NbrOfDiscConversion   = 1;

    AdcHandle.Init.ExternalTrigConv      = ADC_EXTERNALTRIGCONV_T2_CC2; 
    if (HAL_ADC_Init(&AdcHandle) != HAL_OK)
    {
        /* ADC initialization error */
        Error_Handler();
    }

    sConfig.Channel      = ADC_CHANNEL_4;
    sConfig.Rank         = ADC_REGULAR_RANK_1;
    sConfig.SamplingTime = ADC_SAMPLETIME_1CYCLE_5;
  
    if (HAL_ADC_ConfigChannel(&AdcHandle, &sConfig) != HAL_OK)
    {
        /* Channel Configuration Error */
        Error_Handler();
  }


}

static void adc_dma_hal_init(ADC_HandleTypeDef *hadc) {
    GPIO_InitTypeDef          GPIO_InitStruct;
    static DMA_HandleTypeDef  DmaHandle;
    RCC_PeriphCLKInitTypeDef  PeriphClkInit;

    __HAL_RCC_GPIOA_CLK_ENABLE();

    /* Enable clock of ADCx peripheral */
    __HAL_RCC_ADC1_CLK_ENABLE();

    PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC;
    PeriphClkInit.AdcClockSelection = RCC_ADCPCLK2_DIV6;
    HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit);

    /* Enable clock of DMA associated to the peripheral */
    __HAL_RCC_DMA1_CLK_ENABLE();

    GPIO_InitStruct.Pin = GPIO_PIN_4;
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* Configure DMA parameters */
    DmaHandle.Instance = DMA1_Channel1;
  
    DmaHandle.Init.Direction           = DMA_PERIPH_TO_MEMORY;
    DmaHandle.Init.PeriphInc           = DMA_PINC_DISABLE;
    DmaHandle.Init.MemInc              = DMA_MINC_ENABLE;
    DmaHandle.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;   /* Transfer from ADC by half-word to match with ADC configuration: ADC resolution 10 or 12 bits */
    DmaHandle.Init.MemDataAlignment    = DMA_MDATAALIGN_HALFWORD;   /* Transfer to memory by half-word to match with buffer variable type: half-word */
    DmaHandle.Init.Mode                = DMA_NORMAL;//DMA_CIRCULAR;              /* DMA in circular mode to match with ADC configuration: DMA continuous requests */
    DmaHandle.Init.Priority            = DMA_PRIORITY_HIGH;

    /* Deinitialize  & Initialize the DMA for new transfer */
    HAL_DMA_DeInit(&DmaHandle);
    HAL_DMA_Init(&DmaHandle);

    /* Associate the initialized DMA handle to the ADC handle */
    __HAL_LINKDMA(hadc, DMA_Handle, DmaHandle);

    /*##-4- Configure the NVIC #################################################*/

    /* NVIC configuration for DMA interrupt (transfer completion or error) */
    /* Priority: high-priority */
    HAL_NVIC_SetPriority(DMA1_Channel1_IRQn, 1, 0);
    HAL_NVIC_EnableIRQ(DMA1_Channel1_IRQn);

    /* NVIC configuration for ADC interrupt */
    /* Priority: high-priority */
    HAL_NVIC_SetPriority(ADC1_2_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(DMA1_Channel1_IRQn);
}
