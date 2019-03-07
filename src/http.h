//
//  i2c_hw.c
//  sps30
//
//  Created by Benjamin Rannow on 02.02.19.
//  Copyright © 2019 Benjamin Rannow. All rights reserved.
//

#ifndef http_h
#define http_h

/**
 * open the i2c bus
 *
 * path: system /dev/XXXXX address
 *
 * Return: 0 if success | -1 if failed
 */
int8_t http_send_post_request(const char *url, const char *postName, const char *data);

#endif /* i2c_hw_h */
