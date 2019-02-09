//
//  main.c
//  sps30
//
//  Created by Benjamin Rannow on 31.01.19.
//  Copyright © 2019 Benjamin Rannow. All rights reserved.
//

#include <stdio.h>
#include <unistd.h>

#include "sps30.h"

int main(int argc, const char * argv[])
{
    bool newData = false;
    char serial[SPS_MAX_ASCII_LEN];
    
    sps30_init();
    sps30_reset();
    
    printf("starting sensor\n");
    sps30_start();
    printf("sensor started\n");
    
    
    sps30_stop();
    
    return 0;
}
