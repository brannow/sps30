//
//  shdcl.c
//  sps30
//
//  Created by Benjamin Rannow on 31.01.19.
//  Copyright © 2019 Benjamin Rannow. All rights reserved.
//

#include "shdcl.h"

/**
 * calculate the mosi checksum
 *
 * mosi: the mosi frame, checksum will be placed in the frame
 *
 * Return: 0 if success or negative error value
 */
int16_t crcMosiFrame(struct *mosi)
{
    // sum up
    uint8_t sum = mosi.address;
    sum += mosi.command;
    sum += mosi.block_size;
    for (int i = 0; i < mosi.block_size; i++) {
        sum += mosi.block[i];
    }

    // invert checksum
    mosi.checksum = ~sum;
    
    return 0;
}

/**
 * calculate the mosi checksum
 *
 * mosi: the miso frame
 *
 * Return: 1 (true) if success or 0 (false) if failed
 */
bool crcMisoFrame(struct miso)
{
    // sum up
    uint8_t sum = miso.address;
    sum += miso.command;
    sum += miso.state;
    sum += miso.block_size;
    for (int i = 0; i < miso.block_size; i++) {
        sum += miso.block[i];
    }
    
    // invert checksum
    if (miso.checksum == ~sum) {
        return true;
    } else {
        return false;
    }
}
