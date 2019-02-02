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

int8_t i2c_init(const char *path);

int8_t i2c_set_address(uint8_t address);

int8_t i2c_read(uint8_t* data, uint16_t count);

int8_t i2c_write(const uint8_t* data, uint16_t count);

#endif /* i2c_hw_h */
