//
//  shdcl.h
//  sps30
//
//  Created by Benjamin Rannow on 31.01.19.
//  Copyright © 2019 Benjamin Rannow. All rights reserved.
//

#ifndef shdcl_h
#define shdcl_h

#include <stdio.h>
#include <stdbool.h>

#define MAX_BLOCK_SIZE 256
// [0x7E] [0x00] [0x00] [0x02] [0x01 0x03] [0xF9] [0x7E]
struct mosi {                           // sending frame
    uint8_t start;                      // start 0x7E
    uint8_t address;                    // slave address
    uint8_t command;                    // command
    uint8_t block_size;                 // data length
    uint8_t block [MAX_BLOCK_SIZE];     // contrains the data
    uint8_t checksum;                   // checksum
    uint8_t stop;                      // stop 0x7E
};

// [0x7E] [0x00] [0x00] [0x00] [0x00] [] [0xFF] [0x7E]
struct miso {                           // receiving frame
    uint8_t start;                      // start 0x7E
    uint8_t address;                    // slave address
    uint8_t command;                    // command
    uint8_t state;                      // state
    uint8_t block_size;                 // data length
    uint8_t block [MAX_BLOCK_SIZE];     // contrains the data
    uint8_t checksum;                   // checksum
    uint8_t stop;                      // stop 0x7E
};

#define STATE_OK                    0x0  // OK
#define STATE_INVALID_BLOCK_SITE    0x1  // invalid block size
#define STATE_INVALID_COMMAND       0x2  // unknown command
#define STATE_ACCESS_DENIED         0x3  // no access rights for command
#define STATE_INVALID_PARAMETER     0x4  // invalid command parameter or out of range
#define STATE_INTERNAL_ERROR        0x28 // internal function argument invalid or out of range
#define STATE_RUNTIME_ERROR         0x43 // command currently not allowed

/**
 * calculate the mosi checksum
 *
 * mosi: the mosi frame, checksum will be placed in the frame
 *
 * Return: 0 if success or negative error value
 */
int16_t crcMosiFrame(struct *mosi);

/**
 * calculate the mosi checksum
 *
 * mosi: the miso frame
 *
 * Return: 1 (true) if success or 0 (false) if failed
 */
bool crcMisoFrame(struct miso);

#endif /* shdcl_h */
