//
//  i2c_hw.h
//  sps30
//
//  Created by Benjamin Rannow on 02.02.19.
//  Copyright © 2019 Benjamin Rannow. All rights reserved.
//

#ifndef i2c_hw_h
#define i2c_hw_h

#include <stdio.h>
#include <stdint.h>

/**
 * open the i2c bus
 *
 * path: system /dev/XXXXX address
 *
 * Return: 0 if success | -1 if failed
 */
int8_t i2c_init(const char *path);

/**
 * set the i2c slave address
 *
 * address: i2c slave address | i2cdetect -y 1
 *
 * Return: 0 if success | -1 if failed
 */
int8_t i2c_set_address(uint8_t address);

/**
 * Read on i2c bus
 *
 * data: byte buffer
 * count: byte buffer size
 *
 * Return: 0 if success | -1 if failed
 */
int8_t i2c_read(uint8_t* data, uint16_t count);

/**
 * Write on i2c bus
 *
 * data: byte buffer
 * count: byte buffer size
 *
 * Return: 0 if success | -1 if failed
 */
int8_t i2c_write(const uint8_t* data, uint16_t count);

/**
 * Close the i2c data handle
 * after this a new init is required
 */
int8_t i2c_close(void);

#endif /* i2c_hw_h */
