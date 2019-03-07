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
#include <string.h>
#include <time.h>

#include "arg_parser.h"
#include "sps30.h"

#define OUTPUT_BUFFER_SINGLE_SIZE 256
#define OUTPUT_BUFFER_GROUP_SIZE (OUTPUT_BUFFER_SINGLE_SIZE * MAX_SCANNING_TIME_SECONDS)

void signalHandler(int signalCode);
void initApplication(int argc, char * argv[]);
void stopAppication(void);

void startSensor(void);
void gatherSensorData(void);

void outputSensorData(void);
void transformSensorDataToRaw(struct sensorData sd, char *output);
void transformSensorDataToJson(struct sensorData sd, char *output);
void caluclateAVGSensorData(struct sensorData *sd);
void writeOutputIntoFile(const char *, const char *, const char *);
void setupFanDustClean(void);

static struct arg_parser_arguments args;
static uint8_t sensorStatus = 0;

static struct sensorData *data;
static uint16_t sensorDataLimit = 0;
static uint16_t sensorDataCurrentIndex = 0;

/**
 *
 */
int main(int argc, char * argv[])
{
    // first call init application
    initApplication(argc, argv);
    startSensor();
    
    if (args.forceClean == 1 || args.readClean == 1 || args.writeClean != 0) {
        setupFanDustClean();
        stopAppication(); // exit
    }
    
    if (args.time > 0) {
        sensorDataLimit = args.time;
    } else {
        sensorDataLimit = 1;
    }
    
    data = (struct sensorData *)malloc(sizeof(struct sensorData) * sensorDataLimit);
    printf("warmup... \n");
    sleep(3);
    gatherSensorData();
    
    if (data != NULL) {
        free(data);
    }
    
    // exit
    stopAppication();
    return 0;
}

void startSensor()
{
    if (sensorStatus == 0) {
        if (sps30_init() == 0 && sps30_start() == 0) {
            sensorStatus = 1;
        } else {
            printf("failed to init sensor\n");
            stopAppication();
        }
    } else {
        printf("sensor already running, continue \n");
    }
}

void gatherSensorData()
{
    if (sensorStatus == 1) {
        while (1) {
            sleep(1);
            bool isReady = false;
            sps30_isNewDataAvailable(&isReady);
            if (isReady) {
                if (sps30_getSensorData(&data[sensorDataCurrentIndex]) == 0) {
                    
                    sensorDataCurrentIndex = sensorDataCurrentIndex + 1;
                    // limit reached - output data and reset counter
                    if (sensorDataCurrentIndex == sensorDataLimit) {
                        // output data
                        outputSensorData();
                        sensorDataCurrentIndex = 0;
                    }
                    
                    // exit for onetime print time is not set in arguments
                    if (args.time == 0) {
                        return;
                    }
                } else {
                    printf("failed to load sensor data");
                }
            } else {
                printf("no new sensor data available (maybe bus is unstable)");
            }
        }
    }
}

void outputSensorData()
{
    char *outputBuffer;
    if (args.avg == 1 || sensorDataLimit == 1) {
        outputBuffer = (char *)malloc(sizeof(char) * OUTPUT_BUFFER_SINGLE_SIZE);
        outputBuffer[0] = '\0';
        // get the avg of the sensorData
        struct sensorData sd;
        if (sensorDataLimit == 1) {
            sd = data[0];
        } else {
            caluclateAVGSensorData(&sd);
        }
        
        if (args.json == 1) {
            transformSensorDataToJson(sd, outputBuffer);
        } else {
            transformSensorDataToRaw(sd, outputBuffer);
        }
        
    } else {
        outputBuffer = (char *)malloc(sizeof(char) * OUTPUT_BUFFER_GROUP_SIZE);
        outputBuffer[0] = '\0';
        char *subBuffer = (char *)malloc(sizeof(char) * OUTPUT_BUFFER_SINGLE_SIZE);
        subBuffer[0] = '\0';
        if (args.json == 1) {
            outputBuffer[0] = '[';
            outputBuffer[1] = '\0';
            uint16_t sdlMinusOne = sensorDataLimit - 1;
            for (uint16_t i = 0; i < sensorDataLimit; i = i + 1) {
                transformSensorDataToJson(data[i], subBuffer);
                strcat(outputBuffer, subBuffer);
                if (i < sdlMinusOne) {
                    strcat(outputBuffer, ",");
                }
            }
            strcat(outputBuffer, "]");
        } else {
            for (uint16_t i = 0; i < sensorDataLimit; i = i + 1) {
                transformSensorDataToRaw(data[i], subBuffer);
                strcat(outputBuffer, subBuffer);
                strcat(outputBuffer, "\n");
            }
        }
        
        free(subBuffer);
    }
    
    // output data
    if ((args.url == NULL && args.file == NULL) || args.verbose == 1) {
        printf(outputBuffer);
        printf("\n");
    }
    
    if (args.file != NULL) {
        if (args.file_append == 1) {
            writeOutputIntoFile(args.file, "a",outputBuffer);
        } else {
            writeOutputIntoFile(args.file, "w+",outputBuffer);
        }
    }
    if (args.url != NULL) {
        //TODO: send to server
    }
    
    free(outputBuffer);
}

void writeOutputIntoFile(const char *filePath, const char *mode, const char *output)
{
    FILE *f = fopen(filePath, mode);
    if (f) {
        fputs(output,f);
        fputs("\n",f);
        fclose(f);
    }
}

void transformSensorDataToRaw(struct sensorData sd, char *output)
{
    sprintf(output,
            "time %lu\n"
            "0.3-1μm %0.2fμg/m3\n"
            "0.3-2.5μm %0.2fμg/m3\n"
            "0.3-4μm %0.2fμg/m3\n"
            "0.3-10μm %0.2fμg/m3\n"
            "0.3-0.5μm %0.2f/cm3\n"
            "0.3-1μm %0.2f/cm3\n"
            "0.3-2.5μm %0.2f/cm3\n"
            "0.3-4μm %0.2f/cm3\n"
            "0.3-10μm %0.2f/cm3\n"
            "avg %0.2fμm\0",
            (unsigned long)time(NULL),
            sd.mass_particlemc_010pm,
            sd.mass_particlemc_025pm,
            sd.mass_particlemc_040pm,
            sd.mass_particlemc_100pm,
            sd.count_particle_005pm,
            sd.count_particle_010pm,
            sd.count_particle_025pm,
            sd.count_particle_040pm,
            sd.count_particle_100pm,
            sd.avg_particle_size
            );
}

void transformSensorDataToJson(struct sensorData sd, char *output)
{
    sprintf(output,
            "{"
            "\"m_003-010\": %0.2f,"
            "\"m_003-025\": %0.2f,"
            "\"m_003-040\": %0.2f,"
            "\"m_003-100\": %0.2f,"
            "\"c_003-005\": %0.2f,"
            "\"c_003-010\": %0.2f,"
            "\"c_003-025\": %0.2f,"
            "\"c_003-040\": %0.2f,"
            "\"c_003-100\": %0.2f,"
            "\"avg\": %0.2f,"
            "\"ts\": %lu"
            "}\0",
            sd.mass_particlemc_010pm,
            sd.mass_particlemc_025pm,
            sd.mass_particlemc_040pm,
            sd.mass_particlemc_100pm,
            sd.count_particle_005pm,
            sd.count_particle_010pm,
            sd.count_particle_025pm,
            sd.count_particle_040pm,
            sd.count_particle_100pm,
            sd.avg_particle_size,
            (unsigned long)time(NULL)
            );
}

void caluclateAVGSensorData(struct sensorData *sd)
{
    struct sensorData tmpData;
    tmpData.mass_particlemc_010pm = data[0].mass_particlemc_010pm;
    tmpData.mass_particlemc_025pm = data[0].mass_particlemc_025pm;
    tmpData.mass_particlemc_040pm = data[0].mass_particlemc_040pm;
    tmpData.mass_particlemc_100pm = data[0].mass_particlemc_100pm;
    tmpData.count_particle_005pm = data[0].count_particle_005pm;
    tmpData.count_particle_010pm = data[0].count_particle_010pm;
    tmpData.count_particle_025pm = data[0].count_particle_025pm;
    tmpData.count_particle_040pm = data[0].count_particle_040pm;
    tmpData.count_particle_100pm = data[0].count_particle_100pm;
    tmpData.avg_particle_size = data[0].avg_particle_size;
    
    for (uint16_t i = 1; i < sensorDataLimit; i = i + 1) {
        tmpData.mass_particlemc_010pm = tmpData.mass_particlemc_010pm + data[i].mass_particlemc_010pm;
        tmpData.mass_particlemc_025pm = tmpData.mass_particlemc_025pm + data[i].mass_particlemc_025pm;
        tmpData.mass_particlemc_040pm = tmpData.mass_particlemc_040pm + data[i].mass_particlemc_040pm;
        tmpData.mass_particlemc_100pm = tmpData.mass_particlemc_100pm + data[i].mass_particlemc_100pm;
        tmpData.count_particle_005pm = tmpData.count_particle_005pm + data[i].count_particle_005pm;
        tmpData.count_particle_010pm = tmpData.count_particle_010pm + data[i].count_particle_010pm;
        tmpData.count_particle_025pm = tmpData.count_particle_025pm + data[i].count_particle_025pm;
        tmpData.count_particle_040pm = tmpData.count_particle_040pm + data[i].count_particle_040pm;
        tmpData.count_particle_100pm = tmpData.count_particle_100pm + data[i].count_particle_100pm;
        tmpData.avg_particle_size = tmpData.avg_particle_size + data[i].avg_particle_size;
    }
    
    sd->mass_particlemc_010pm = tmpData.mass_particlemc_010pm / sensorDataLimit;
    sd->mass_particlemc_025pm = tmpData.mass_particlemc_025pm / sensorDataLimit;
    sd->mass_particlemc_040pm = tmpData.mass_particlemc_040pm / sensorDataLimit;
    sd->mass_particlemc_100pm = tmpData.mass_particlemc_100pm / sensorDataLimit;
    sd->count_particle_005pm = tmpData.count_particle_005pm / sensorDataLimit;
    sd->count_particle_010pm = tmpData.count_particle_010pm / sensorDataLimit;
    sd->count_particle_025pm = tmpData.count_particle_025pm / sensorDataLimit;
    sd->count_particle_040pm = tmpData.count_particle_040pm / sensorDataLimit;
    sd->count_particle_100pm = tmpData.count_particle_100pm / sensorDataLimit;
    sd->avg_particle_size = tmpData.avg_particle_size / sensorDataLimit;
}

void setupFanDustClean()
{
    if (args.forceClean == 1) {
        printf("start fan cleaning, please wait...\n");
        if (sps30_startFanCleaning() == 0) {
            printf("done, shutdown sensor.\n");
        } else {
            printf("failed to start fan cleaning mode.\n");
        }
        return;
    }
    
    if (args.readClean == 1 || args.writeClean != 0) {
        printf("read fan clean interval, please wait...\n");
        uint32_t seconds = 0;
        if (sps30_getFanAutoCleanInterval(&seconds) == 0) {
            sleep(2);
            printf("auto dusk fan clean Interval: %d seconds (%0.1f days) \n", seconds, (float)seconds / 86400);
        } else {
            printf("failed to read fan cleaning interval.\n");
            return;
        }
    }
    
    if (args.writeClean >= 9000) {
        printf("write fan clean interval, please wait...\n");
        if (sps30_setFanAutoCleanInterval(args.writeClean) == 0) {
            sleep(2);
            printf("set auto clean interval to Interval: %d seconds (%0.1f days)\n",args.writeClean, (float)args.writeClean / 86400);
        }else {
            printf("failed to write fan cleaning interval.\n");
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
    
    printf("output:\navg: %d\njson: %d\nappend: %d\ntime: %d\n \nfile: %s\nurl: %s\npostName: %s\nverbose: %d\nr: %d\nc: %d\nw: %d\n\n", args.avg, args.json, args.file_append, args.time, args.file, args.url, args.url_post_name, args.verbose, args.readClean, args.forceClean, args.writeClean);
    
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
    sps30_stop();
    sps30_reset();
    exit(1);
}
