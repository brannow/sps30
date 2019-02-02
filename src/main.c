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
    sps30_init();
    
    char sen[SPS_MAX_SERIAL_LEN];
    sps30_getSerialNumber(sen);
    
    printf("%s", sen);
    
    return 0;
}
