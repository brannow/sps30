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

#include "sps30.h"

void signalHandler(int signalCode)
{
    sps30_stop();
    exit(1);
}



int main(int argc, const char * argv[])
{
    // signal handler
    struct sigaction sigIntHandler;
    sigIntHandler.sa_handler = signalHandler;
    sigemptyset(&sigIntHandler.sa_mask);
    sigIntHandler.sa_flags = 0;
    sigaction(SIGINT, &sigIntHandler, NULL);
    
    // sensor init
    sps30_init();
    sps30_reset();
    sps30_start();
    
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
    
    /*bool isReady = false;
    struct sensorData data;
    
    while (1) {
        isReady = false;
        sps30_isNewDataAvailable(&isReady);
        if (isReady) {
            if (sps30_getSensorData(&data) == 0) {
                printf("measured values:\n"
                       "MASS: \n"
                       "\t <=1μm particles | %0.2fμg / m3 \n"
                       "\t 2.5μm particles | %0.2fμg / m3\n"
                       "\t 4μm particles\t | %0.2fμg / m3\n"
                       "\t 10μm particles\t | %0.2fμg / m3\n"
                       "Count: \n"
                       "\t0.5μm particles\t | %0.2f per cm3 \n"
                       "\t1μm particles\t | %0.2f per cm3\n"
                       "\t2.5μm particles\t | %0.2f per cm3\n"
                       "\t4.5μm particles\t | %0.2f per cm3\n"
                       "\t10μm particles\t | %0.2f per cm3\n"
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
        sleep(1);
    }*/
    
    // exit
    signalHandler(0);
    
    return 0;
}
