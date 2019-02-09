//
//  sps30.c
//  sps30
//
//  Created by Benjamin Rannow on 31.01.19.
//  Copyright © 2019 Benjamin Rannow. All rights reserved.
//

#include "sps30.h"
#include "sensirion.h"

#pragma mark - Const

#define SPS_CMD_START_MEASUREMENT       0x0010
#define SPS_CMD_STOP_MEASUREMENT        0x0104
#define SPS_CMD_READ_MEASUREMENT        0x0300
#define SPS_CMD_GET_DATA_READY          0x0202
#define SPS_CMD_AUTOCLEAN_INTERVAL      0x8004
#define SPS_CMD_FAN_CLEAN               0x5607
#define SPS_CMD_GET_SERIAL              0xd033
#define SPS_CMD_GET_PRODUCT             0xd025
#define SPS_CMD_RESET                   0xd304
#define SPS_WRITE_DELAY                  20000

static const uint8_t SPS30_I2C_ADDRESS = 0x69;

int8_t sps30_getAsciiData(uint16_t cmd, char *data);

#pragma mark - Sensor Controls

/**
 * check if sensor functional
 * Return: 0 if success or negative sensor error code
 */
int8_t sps30_init()
{
    if (sensirion_init("/dev/i2c-1", SPS30_I2C_ADDRESS) == 0) {
        char serialNumber[SPS_MAX_ASCII_LEN];
        return sps30_getSerialNumber(serialNumber);
    }
    
    return -1;
}

/**
 * read sensor serial number
 *
 * serialNumber: string is 0x0 terminated
 *
 * Return: 0 if success or negative sensor error code
 */
int8_t sps30_getSerialNumber(char *serialNumber)
{
    return sps30_getAsciiData(SPS_CMD_GET_SERIAL, serialNumber);
}

/**
 * read sensor Article number
 *
 * articalNumber: string is 0x0 terminated
 *
 * Return: 0 if success or negative sensor error code
 */
int8_t sps30_getProductNumber(char *productNumber)
{
    return sps30_getAsciiData(SPS_CMD_GET_SERIAL, productNumber);
}

/**
 * internal read ascii data
 *
 * cmd: sensor command
 * data: string is 0x0 terminated after reading
 *
 * Return: 0 if success or negative sensor error code
 */
int8_t sps30_getAsciiData(uint16_t cmd, char *data)
{
    uint16_t i;
    int16_t ret;
    union {
        char data[SPS_MAX_ASCII_LEN];
        uint16_t __enforce_alignment;
    } buffer;
    
    ret = sensirion_read_cmd(cmd, (uint16_t *)buffer.data, SENSIRION_NUM_WORDS(buffer.data));
    if (ret != STATUS_OK)
        return ret;
    
    SENSIRION_WORDS_TO_BYTES(buffer.data, SENSIRION_NUM_WORDS(buffer.data));
    for (i = 0; i < SPS_MAX_ASCII_LEN; ++i) {
        data[i] = buffer.data[i];
        if (data[i] == '\0')
            return 0;
    }
    
    return 0;
}

/**
 * start messurement
 *
 * Return: 0 if success or negative sensor error code
 */
int8_t sps30_start()
{
    // argument expected to be 3 byte long (0x03 0x00 CRC)
    // 0x03 0x00 (crc is added in sensirion module)
    const uint16_t arg = (0x03 << 8) | 0x00;
    return sensirion_write_cmd_with_args(SPS_CMD_START_MEASUREMENT, &arg, SENSIRION_NUM_WORDS(arg));
}

/**
 * stop messurement
 *
 * Return: 0 if success or negative sensor error code
 */
int8_t sps30_stop()
{
    return sensirion_write_cmd(SPS_CMD_STOP_MEASUREMENT);
}

/**
 * reset sensor
 * Return: 0 if success or negative sensor error code
 */
int8_t sps30_reset() {
    return sensirion_write_cmd(SPS_CMD_RESET);
}

#pragma mark - Messurement

/**
 * if new (since last pull) data available
 *
 * check: true | false (0|1)
 *
 * Return: 0 if success or negative sensor error code
 */
int8_t sps30_isNewDataAvailable(bool *check)
{
    
}

/**
 * get the newest sensor data
 *
 * data: struct sensorData // see: sps30.h
 *
 * Return: 0 if success or negative sensor error code
 */
int8_t sps30_getSensorData(struct sensorData *data)
{
    
}

#pragma mark - Dust Cleaning

/**
 * Start fan Cleaning manually
 * NOTE: CAN ONLY RUNNED AFTER MESUREMENT IS STARTED
 *
 * Reset Fan Cleaning Internal Interval to 0
 *
 * Return 0 if success or negative sensor error code
 */
int8_t sps30_startFanCleaning()
{
    return sensirion_write_cmd(SPS_CMD_FAN_CLEAN);
}

/**
 * get the auto cleaning interval,
 * auto-cleaning starts fan at full speed for 10 seconds
 * default: 604800 seconds (1 week)
 *
 * NOTE: if the sensor is off (power off) the internal clock is reseted to 0!
 * make sure that the sensor is cleaned at least once a week
 *
 * seconds: auto cleaning interval
 *
 * Return: 0 if success or negative sensor error code
 */
int8_t sps30_getFanAutoCleanInterval(uint32_t *seconds)
{
    
}

/**
 * set the auto cleaning interval,
 * auto-cleaning starts fan at full speed for 10 seconds
 * default: 604800 seconds (1 week)
 *
 * NOTE: if the sensor is off (power off) the internal clock is reseted to 0!
 * make sure that the sensor is cleaned at least once a week
 *
 * seconds: auto cleaning interval
 *
 * Return: 0 if success or negative sensor error code
 */
int8_t sps30_setFanAutoCleanInterval(uint32_t seconds)
{
    
}
