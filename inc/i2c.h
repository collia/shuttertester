/**
 * @file    i2c.h
 * @author  Nikolay
 * @license MIT
 * @date    2020-07-11
 * @brief   File wrapper for i2c interface
 *
 * 
 */

#include <stdint.h>

int i2c_master_init();
int i2c_read_byte(uint8_t dev, uint8_t reg);
int i2c_write_byte(uint8_t dev, uint8_t reg, const uint8_t data);
