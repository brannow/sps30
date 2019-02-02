//
//  sps30.c
//  sps30
//
//  Created by Benjamin Rannow on 31.01.19.
//  Copyright © 2019 Benjamin Rannow. All rights reserved.
//

#include "sps30.h"
#include "sensirion.h"

#define SPS_CMD_START_MEASUREMENT       0x0010
#define SPS_CMD_START_MEASUREMENT_ARG   0x0300
#define SPS_CMD_STOP_MEASUREMENT        0x0104
#define SPS_CMD_READ_MEASUREMENT        0x0300
#define SPS_CMD_GET_DATA_READY          0x0202
#define SPS_CMD_AUTOCLEAN_INTERVAL      0x8004
#define SPS_CMD_GET_SERIAL              0xd033
#define SPS_CMD_RESET                   0xd304
#define SPS_WRITE_DELAY_US              20000

static const uint8_t SPS30_I2C_ADDRESS = 0x69;

int8_t sps30_init()
{
    return sensirion_init("/dev/i2c-1", SPS30_I2C_ADDRESS);
}

int8_t sps30_getSerialNumber(char *serialNumber)
{
    uint16_t i;
    int16_t ret;
    union {
        char serialNumber[SPS_MAX_SERIAL_LEN];
        uint16_t __enforce_alignment;
    } buffer;
    
    ret = sensirion_read_cmd(SPS_CMD_GET_SERIAL, (uint16_t *)buffer.serialNumber, SENSIRION_NUM_WORDS(buffer.serialNumber));
    if (ret != STATUS_OK)
        return ret;
    
    SENSIRION_WORDS_TO_BYTES(buffer.serialNumber, SENSIRION_NUM_WORDS(buffer.serialNumber));
    for (i = 0; i < SPS_MAX_SERIAL_LEN; ++i) {
        serialNumber[i] = buffer.serialNumber[i];
        if (serialNumber[i] == '\0')
            return 0;
    }
    
    return 0;
}
