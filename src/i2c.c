/**
 * @file    i2c.c
 * @author  Nikolay
 * @license MIT
 * @date    2020-07-11
 * @brief   File wrapper for i2c interface
 *
 * 
 */
#include "i2c.h"
#include <stdint.h>
#include "stm32f1xx_hal.h"
#include "stm32f1xx_hal_i2c.h"

#define ST_I2C_BUS     I2C1
#define ST_I2C_SPEED   400000

#define I2C_SCL_PIN                       GPIO_PIN_6        /* PB.06*/
#define I2C_SCL_GPIO_PORT                 GPIOB
#define I2C_SDA_PIN                       GPIO_PIN_7        /* PB.07*/
#define I2C_SDA_GPIO_PORT                 GPIOB

#define I2C_CLK_ENABLE()                  __HAL_RCC_I2C1_CLK_ENABLE()
#define I2C_SDA_GPIO_CLK_ENABLE()         __HAL_RCC_GPIOB_CLK_ENABLE()
#define I2C_SCL_GPIO_CLK_ENABLE()         __HAL_RCC_GPIOB_CLK_ENABLE()
#define I2C_FORCE_RESET()                 __HAL_RCC_I2C1_FORCE_RESET()
#define I2C_RELEASE_RESET()               __HAL_RCC_I2C1_RELEASE_RESET()


static void i2c_msp_init(I2C_HandleTypeDef *hi2c);
static void i2c_error(uint8_t addr);

static const uint32_t i2c_timeout = 3000;
I2C_HandleTypeDef heval_I2c;


int i2c_master_init() {
    if(HAL_I2C_GetState(&heval_I2c) == HAL_I2C_STATE_RESET)
    {
        heval_I2c.Instance              = ST_I2C_BUS;
        heval_I2c.Init.ClockSpeed       = ST_I2C_SPEED;
        heval_I2c.Init.DutyCycle        = I2C_DUTYCYCLE_2;
        heval_I2c.Init.OwnAddress1      = 0;
        heval_I2c.Init.AddressingMode   = I2C_ADDRESSINGMODE_7BIT;
        heval_I2c.Init.DualAddressMode  = I2C_DUALADDRESS_DISABLE;
        heval_I2c.Init.OwnAddress2      = 0;
        heval_I2c.Init.GeneralCallMode  = I2C_GENERALCALL_DISABLE;
        heval_I2c.Init.NoStretchMode    = I2C_NOSTRETCH_DISABLE;

        /* Init the I2C */
        i2c_msp_init(&heval_I2c);
        HAL_I2C_Init(&heval_I2c);
  }
    return 0;
}

int i2c_read_byte(uint8_t dev, uint8_t reg) {
    HAL_StatusTypeDef status = HAL_OK;
    uint8_t value = 0;
  
    status = HAL_I2C_Mem_Read(&heval_I2c, dev, reg, I2C_MEMADD_SIZE_8BIT, &value, 1, i2c_timeout);
  
    /* Check the communication status */
    if(status != HAL_OK)
    {
        /* Execute user timeout callback */
        i2c_error(dev);
        return -1;

    }
    return value;
    
}

int i2c_write_byte(uint8_t dev, uint8_t reg, const uint8_t data) {
    HAL_StatusTypeDef status = HAL_OK;
  
    status = HAL_I2C_Mem_Write(&heval_I2c, dev, reg, I2C_MEMADD_SIZE_8BIT, &data, 1, i2c_timeout);
  
    /* Check the communication status */
    if(status != HAL_OK)
    {
        /* Execute user timeout callback */
        i2c_error(dev);
        return -1;

    }
    return 0;
}



static void i2c_msp_init(I2C_HandleTypeDef *hi2c)
{   
  GPIO_InitTypeDef  gpioinitstruct = {0};

  if (hi2c->Instance == ST_I2C_BUS)
  {
    /*## Configure the GPIOs ################################################*/

    /* Enable GPIO clock */
    I2C_SDA_GPIO_CLK_ENABLE();
    I2C_SCL_GPIO_CLK_ENABLE();

    /* Configure I2C Tx as alternate function  */
    gpioinitstruct.Pin       = I2C_SCL_PIN;
    gpioinitstruct.Mode      = GPIO_MODE_AF_OD;
    gpioinitstruct.Pull      = GPIO_NOPULL;
    gpioinitstruct.Speed     = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(I2C_SCL_GPIO_PORT, &gpioinitstruct);

    /* Configure I2C Rx as alternate function  */
    gpioinitstruct.Pin = I2C_SDA_PIN;
    HAL_GPIO_Init(I2C_SDA_GPIO_PORT, &gpioinitstruct);

    /*## Configure the Eval I2Cx peripheral #######################################*/
    /* Enable Eval_I2Cx clock */
    I2C_CLK_ENABLE();

    /* Add delay related to RCC workaround */
    while (READ_BIT(RCC->APB1ENR, RCC_APB1ENR_I2C1EN) != RCC_APB1ENR_I2C1EN) {};

    /* Force the I2C Periheral Clock Reset */
    I2C_FORCE_RESET();

    /* Release the I2C Periheral Clock Reset */
    I2C_RELEASE_RESET();
  
    /* Enable and set Eval I2Cx Interrupt to the highest priority */
    //HAL_NVIC_SetPriority(EVAL_I2Cx_EV_IRQn, 0xE, 0);
    //HAL_NVIC_EnableIRQ(EVAL_I2Cx_EV_IRQn);

    /* Enable and set Eval I2Cx Interrupt to the highest priority */
    //HAL_NVIC_SetPriority(EVAL_I2Cx_ER_IRQn, 0xE, 0);
    //HAL_NVIC_EnableIRQ(EVAL_I2Cx_ER_IRQn);
  }
}


static void i2c_error(uint8_t addr)
{
  /* De-initialize the IOE comunication BUS */
  HAL_I2C_DeInit(&heval_I2c);

  /* Re-Initiaize the IOE comunication BUS */
  i2c_master_init();
}

