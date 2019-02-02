//
//  i2c_hw.c
//  sps30
//
//  Created by Benjamin Rannow on 02.02.19.
//  Copyright © 2019 Fuyukai Rannow. All rights reserved.
//

#include <unistd.h>
#include <stdio.h>
#include <stdint.h>
#include <fcntl.h>
#include <sys/ioctl.h>

#include "i2c_hw.h"

#define I2C_SLAVE 0x0703 // default Linux

#define I2C_WRITE_FAILED -1
#define I2C_READ_FAILED -1
#define I2C_READWRITE_SUCCESS 0

static int i2c_handle = 0;
static uint8_t i2c_address = 0;

int8_t i2c_init(const char *path)
{
    i2c_handle = open(path, O_RDWR);
    if (i2c_handle == -1) {
        return -1;
    }
    
    return 0;
}

int8_t i2c_set_address(uint8_t address)
{
    if (i2c_address != address) {
        ioctl(i2c_handle, I2C_SLAVE, address);
        i2c_address = address;
    }
    
    if (i2c_address <= 0) {
        return -1;
    }
    
    return 0;
}

int8_t i2c_read(uint8_t* data, uint16_t count)
{
    if (read(i2c_handle, data, count) != count) {
        return I2C_READ_FAILED;
    }
    
    return I2C_READWRITE_SUCCESS;
}

int8_t i2c_write(const uint8_t* data, uint16_t count)
{    
    if (write(i2c_handle, data, count) != count) {
        return I2C_WRITE_FAILED;
    }
    
    return I2C_READWRITE_SUCCESS;
}
