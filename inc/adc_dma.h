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

typedef void (*adc_dma_stop_clbk)(uint16_t* data);

void adc_dma_init();
void adc_dma_start(uint16_t* data1, uint16_t* data2, uint32_t data_len);
void adc_dma_stop();
void adc_dma_set_stop_callback(adc_dma_stop_clbk clbk);
void adc_dma_adc_irq(void);
void adc_dma_dma_irq(void);
