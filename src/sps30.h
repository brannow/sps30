//
//  sps30.h
//  sps30
//
//  Created by Benjamin Rannow on 31.01.19.
//  Copyright © 2019 Benjamin Rannow. All rights reserved.
//

#ifndef sps30_h
#define sps30_h

#include <stdint.h>
#include <stdbool.h>

#pragma mark - Const

#define SPS_MAX_ASCII_LEN 32

// force bit alignment
#pragma pack(push, 1)
struct sensorData {                 // μg = microgramm!
    float mass_particlemc_010pm;    // pm1      μg/m3
    float mass_particlemc_025pm;    // pm2.5    μg/m3
    float mass_particlemc_040pm;    // pm4      μg/m3
    float mass_particlemc_100pm;    // pm10     μg/m3
    float count_particle_005pm;     // pm0.5    #/cm3
    float count_particle_010pm;     // pm1      #/cm3
    float count_particle_025pm;     // pm2.5    #/cm3
    float count_particle_040pm;     // pm4      #/cm3
    float count_particle_100pm;     // pm10     #/cm3
    float avg_particle_size;        // in micrometer
};
#pragma pack(pop)

#pragma mark - Sensor Controls

/**
 * check if sensor functional
 * Return: 0 if success or negative sensor error code
 */
int8_t sps30_init(void);

/**
 * reset sensor
 * Return: 0 if success or negative sensor error code
 */
int8_t sps30_reset(void);

/**
 * read sensor serial number
 *
 * serialNumber: string is 0x0 terminated
 *
 * Return: 0 if success or negative sensor error code
 */
int8_t sps30_getSerialNumber(char *serialNumber);

/**
 * read sensor Article number
 *
 * articalNumber: string is 0x0 terminated
 *
 * Return: 0 if success or negative sensor error code
 */
int8_t sps30_getProductNumber(char *productNumber);

/**
 * start messurement
 *
 * Return: 0 if success or negative sensor error code
 */
int8_t sps30_start(void);

/**
 * stop messurement
 *
 * Return: 0 if success or negative sensor error code
 */
int8_t sps30_stop(void);

/**
 * TODO: currently SEG_FAULT - DUNNO why -.-
 * DO NOT USE THIS ATM!
 *
 * Close connection to sps30
 * after reusing, a new init is reqired
 */
int8_t sps30_destroy(void);

#pragma mark - Messurement

/**
 * if new (since last pull) data available
 *
 * check: true | false (0|1)
 *
 * Return: 0 if success or negative sensor error code
 */
int8_t sps30_isNewDataAvailable(bool *check);

/**
 * get the newest sensor data
 *
 * data: struct sensorData // see: sps30.h
 *
 * Return: 0 if success or negative sensor error code
 */
int8_t sps30_getSensorData(struct sensorData *data);

#pragma mark - Dust Cleaning

/**
 * Start fan Cleaning manually
 * NOTE: CAN ONLY RUNNED AFTER MESUREMENT IS STARTED
 *
 * Reset Fan Cleaning Internal Interval to 0
 *
 * Return 0 if success or negative sensor error code
 */
int8_t sps30_startFanCleaning(void);

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
int8_t sps30_getFanAutoCleanInterval(uint32_t *seconds);

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
int8_t sps30_setFanAutoCleanInterval(uint32_t seconds);

#endif /* sps30_h */
