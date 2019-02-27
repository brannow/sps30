//
//  arg_parser.h
//  sps30
//
//  Created by Benjamin Rannow on 14.02.19.
//  Copyright © 2019 Benjamin Rannow. All rights reserved.
//

#ifndef arg_parser_h
#define arg_parser_h

#include <stdint.h>

struct arg_parser_arguments {
    uint8_t avg;            // -a, --avg            0 | 1
    uint8_t json;           // -j, --json           0 | 1
    uint8_t file_append;    // -p, --file-append    0 | 1
    
    uint16_t time;          // -t, --time           0-255
    
    char *file;             // -f, --file           string 255
    char *url;              // -u, --url            string 255
    char *url_post_name;    // -n, --url-post-name  string 30
};

// init method
int8_t arg_parser_init(int argc, char **argv, struct arg_parser_arguments *arguments);

#endif /* arg_parser_h */
