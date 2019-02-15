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

enum ARGUMENT_FLAGS {
    FORMAT_RAW = 1 << 0,
    FORMAT_JSON = 1 << 1,
    
    OUTPUT_FILE = 1 << 2,
    OUTPUT_CONSOLE = 1 << 3,
    OUTPUT_SERVER = 1 << 4,
    
    OUTPUT_APPEND = 1 << 5, // is only relevant for file output
};

struct arg_parser_arguments {
    enum ARGUMENT_FLAGS flags;
    uint16_t output_time; // -t
    uint16_t sampling_time; // -s
};

// init method
int8_t arg_parser_init(int argc, char **argv);

#endif /* arg_parser_h */
