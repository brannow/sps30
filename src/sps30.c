//
//  sps30.c
//  sps30
//
//  Created by Benjamin Rannow on 31.01.19.
//  Copyright © 2019 Benjamin Rannow. All rights reserved.
//

#include <string.h> // memcpy
#include <unistd.h> // usleep

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

#define SPS_WRITE_DELAY                 200000

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
    
    return STATUS_FAIL;
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
            return STATUS_OK;
    }
    
    return STATUS_FAIL;
}

/**
 * start messurement
 *
 * Return: 0 if success or negative sensor error code
 */
int8_t sps30_start()
{
    // sensor expected to be 3 byte long (0x03 0x00 CRC) however
    // 0x03 0x00 (crc is added in sensirion module)
    const uint16_t arg = (0x03 << 8) | 0x00;
    int8_t ret = sensirion_write_cmd_with_args(SPS_CMD_START_MEASUREMENT, &arg, SENSIRION_NUM_WORDS(arg));
    
    if (ret != STATUS_OK)
        return ret;
    
    usleep(SPS_WRITE_DELAY);
    return ret;
}

/**
 * stop messurement
 *
 * Return: 0 if success or negative sensor error code
 */
int8_t sps30_stop()
{
    int8_t ret = sensirion_write_cmd(SPS_CMD_START_MEASUREMENT);
    
    if (ret != STATUS_OK)
        return ret;
    
    usleep(SPS_WRITE_DELAY);
    return ret;
}

/**
 * reset sensor
 * Return: 0 if success or negative sensor error code
 */
int8_t sps30_reset()
{
    int8_t ret = sensirion_write_cmd(SPS_CMD_RESET);
    if (ret != STATUS_OK)
        return ret;
    
    usleep(SPS_WRITE_DELAY);
    return ret;
}

/**
 * Experimental
 * do not use
 */
int8_t sps30_destroy()
{
    return sensirion_terminate();
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
    *check = false;
    uint16_t buffer;
    int8_t ret = sensirion_read_cmd(SPS_CMD_GET_DATA_READY,
                           &buffer, SENSIRION_NUM_WORDS(buffer));
    
    if (ret != STATUS_OK)
        return ret;
    
    SENSIRION_WORDS_TO_BYTES(&buffer, SENSIRION_NUM_WORDS(buffer));
    uint8_t bitBuffer[2] = {buffer & 0xff, buffer >> 8};
    if (bitBuffer[0] == 0x00 && bitBuffer[1] == 0x01) {
        *check = true;
    }
    
    return STATUS_OK;
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
    int8_t ret;
    union {
        uint16_t u16[2];
        uint32_t u32;
    } buffer[10];
    
    ret = sensirion_read_cmd(SPS_CMD_READ_MEASUREMENT,
                                 buffer->u16, SENSIRION_NUM_WORDS(buffer));
    if (ret != STATUS_OK)
        return ret;
    
    SENSIRION_WORDS_TO_BYTES(buffer->u16, SENSIRION_NUM_WORDS(buffer));
    for (int i = 0; i < 10; i++) {
        buffer[i].u32 = be32_to_cpu(buffer[i].u32);
    }
    
    memcpy(data, buffer, sizeof buffer);
    return 0;
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
    int8_t ret = sensirion_write_cmd(SPS_CMD_FAN_CLEAN);
    if (ret != STATUS_OK)
        return ret;
    // sleep for 20 seconds to secure the sensor
    sleep(20);
    return ret;
}

/**
 * get the auto cleaning interval,
 * auto-cleaning starts fan at full speed for 10 seconds
 * default: 345600 seconds (4 days)
 *
 * NOTE: if the sensor is off (power off) the internal clock is reseted to 0!
 * make sure that the sensor is cleaned at least once a week
 *
 * after setting a new interval this immediately active,
 * getFanAutoCleanInterval will not return this value until the next start/restart of the sensor
 *
 * seconds: auto cleaning interval
 *
 * Return: 0 if success or negative sensor error code
 */
int8_t sps30_getFanAutoCleanInterval(uint32_t *seconds)
{
    union {
        uint16_t buf[2];
        uint32_t seconds;
    } buffer;
    int8_t ret = sensirion_read_cmd(SPS_CMD_AUTOCLEAN_INTERVAL,
                                     buffer.buf, SENSIRION_NUM_WORDS(buffer));
    if (ret != STATUS_OK)
        return ret;
    
    SENSIRION_WORDS_TO_BYTES(buffer.buf, SENSIRION_NUM_WORDS(buffer));
    *seconds = be32_to_cpu(buffer.seconds);
    
    return 0;
}

/**
 * set the auto cleaning interval,
 * auto-cleaning starts fan at full speed for 10 seconds
 * default: 345600 seconds (4 days)
 *
 * NOTE: if the sensor is off (power off) the internal clock is reseted to 0!
 * make sure that the sensor is cleaned at least once a week
 *
 * after setting a new interval this immediately active,
 * getFanAutoCleanInterval will not return this value until the next start/restart of the sensor
 *
 * seconds: auto cleaning interval
 *
 * Return: 0 if success or negative sensor error code
 */
int8_t sps30_setFanAutoCleanInterval(uint32_t seconds)
{
    const uint16_t buffer[2] = {
        (seconds & 0xFFFF0000) >> 16,
        (seconds & 0x0000FFFF) >> 0
    };
    
    int8_t ret = sensirion_write_cmd_with_args(SPS_CMD_AUTOCLEAN_INTERVAL, buffer,
                                            SENSIRION_NUM_WORDS(buffer));
    usleep(SPS_WRITE_DELAY);
    return ret;
}
