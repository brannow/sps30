//
//  main.c
//  sps30
//
//  Created by Benjamin Rannow on 31.01.19.
//  Copyright © 2019 Benjamin Rannow. All rights reserved.
//

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>

#include "arg_parser.h"
#include "sps30.h"

void signalHandler(int signalCode);
void initApplication(int argc, char * argv[]);
void stopAppication(void);

static struct arg_parser_arguments args;
static uint8_t sensorStatus = 0;

void startSensor();
void gatherSensorData();



/**
 *
 */
int main(int argc, char * argv[])
{
    // first call init application
    initApplication(argc, argv);
    
    startSensor();
    sleep(2);
    gatherSensorData();
    sleep(2);

    /*
    uint32_t seconds = 0;
    if (sps30_getFanAutoCleanInterval(&seconds) == 0) {
        printf("interval: %d \n", seconds);
    } else {
        printf("failed");
    }
    
    sps30_setFanAutoCleanInterval(345600);
    sleep(2);
    
    if (sps30_getFanAutoCleanInterval(&seconds) == 0) {
        printf("interval: %d \n", seconds);
    } else {
        printf("failed");
    }
    */
    // exit
    stopAppication();
    return 0;
}

void startSensor()
{
    if (sensorStatus == 0) {
        if (sps30_init() == 0 && sps30_start() == 0) {
            sensorStatus = 1;
            uint32_t seconds = 0;
            if (sps30_getFanAutoCleanInterval(&seconds) == 0) {
                printf("auto fan clean Interval: %d seconds (%d days) \n", seconds, seconds / 86400);
            }
        } else {
            printf("[ERROR]: fail to init sensor\n");
            stopAppication();
        }
    } else {
        printf("sensor already running \n");
    }
}

void gatherSensorData()
{
    if (sensorStatus == 1) {
        bool isReady = false;
        struct sensorData data;
        sps30_isNewDataAvailable(&isReady);
        if (isReady) {
            if (sps30_getSensorData(&data) == 0) {
                printf("measured values:\n"
                       "MASS: \n"
                       "\t 0.3-1μm particles\t | %0.2fμg / m3 \n"
                       "\t 0.3-2.5μm particles\t | %0.2fμg / m3\n"
                       "\t 0.3-4μm particles\t | %0.2fμg / m3\n"
                       "\t 0.3-10μm particles\t | %0.2fμg / m3\n"
                       "Count: \n"
                       "\t0.3-0.5μm particles\t | %0.2f per cm3 \n"
                       "\t0.3-1μm particles\t | %0.2f per cm3\n"
                       "\t0.3-2.5μm particles\t | %0.2f per cm3\n"
                       "\t0.3-4.5μm particles\t | %0.2f per cm3\n"
                       "\t0.3-10μm particles\t | %0.2f per cm3\n"
                       "\tcurrent average particle size | %0.2fμm\n\n",
                       data.mass_particlemc_010pm, data.mass_particlemc_025pm,
                       data.mass_particlemc_040pm, data.mass_particlemc_100pm,
                       data.count_particle_005pm, data.count_particle_010pm,
                       data.count_particle_025pm, data.count_particle_040pm,
                       data.count_particle_100pm,
                       data.avg_particle_size);
            } else {
                printf("failed to load sensor data");
            }
        } else {
            printf("no new sensor data available (maybe bus is unstable)");
        }
    }
}

/**
 * Initzialize Application
 */
void initApplication(int argc, char * argv[])
{
    // signal handler eg: CTRL + C or CTRL + T
    struct sigaction sigIntHandler;
    sigIntHandler.sa_handler = signalHandler;
    sigemptyset(&sigIntHandler.sa_mask);
    sigIntHandler.sa_flags = 0;
    sigaction(SIGINT, &sigIntHandler, NULL);
    
    // argument parser
    uint8_t ret = arg_parser_init(argc, argv, &args);
    
    printf("output:\navg: %d\njson: %d\nappend: %d\ntime: %d\n \nfile: %s\nurl: %s\npostName: %s\n\n", args.avg, args.json, args.file_append, args.time, args.file, args.url, args.url_post_name);
    
    if (ret < 0) {
        // cannot init argument parsing exit
        printf("argument parsing error %d", ret);
        exit(1);
    }
}

/**
 * Handle all incomming signals such as the termination CTRL + C
 */
void signalHandler(int signalCode)
{
    stopAppication();
}

/**
 * stops sensor and free all memory if needed
 */
void stopAppication(void)
{
    printf("exit(%d/%d)\n", sps30_stop(), sps30_reset());
    exit(1);
}
