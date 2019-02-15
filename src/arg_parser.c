//
//  arg_parser.c
//  sps30
//
//  Created by Benjamin Rannow on 14.02.19.
//  Copyright © 2019 Benjamin Rannow. All rights reserved.
//

#include <stdlib.h>
#include <argp.h>

#include "arg_parser.h"


#define PATH_MAX_LENGTH 251
#define POST_NAME_MAX_LENGTH 21
// file or Server Path
static char *path;
// file or Server Path post name
static char *post_name;

const char *argp_program_version = "sps30 0.1";
const char *argp_program_bug_address = "<benjamin@fuyukai.moe>";

/* Program documentation. */
static char doc[] = "SPS30 I2C Interface with output functionality into file or direct sending to a server";

/* A description of the arguments we accept. */
static char args_doc[] = "";

struct argp_option options[] =
{
    {"interval",        'i', "SECONDS",     OPTION_ARG_OPTIONAL, "Output Interval, in seconds. Default: 1."},
    {"time",            't', "SECONDS",     OPTION_ARG_OPTIONAL, "sample rate timing. Default: 1."},
    {"json",            'j', 0,             OPTION_ARG_OPTIONAL, "Parse the sensor output to JSON."},
    {"path",            'p', "PATH",        OPTION_ARG_OPTIONAL, "Write Sensor Data into the given Filepath, if no file exists create a new. (max 250 chars)"},
    {"append",          'a', 0,             OPTION_ARG_OPTIONAL, "Append data into file, otherwise overwrite."},
    {"url",             'u', "URL",         OPTION_ARG_OPTIONAL, "Send Data over HTTP/S to the server URL as post Message. (max 250 chars)"},
    {"url-post-name",   'n', "NAME",        OPTION_ARG_OPTIONAL, "The URL POST name (max 20 chars). Default: 'data'."},
    {0}
};

static error_t parse_opt (int key, char *arg, struct argp_state *state)
{
    return 0;
}

/* Our argp parser. */
static struct argp argp = { options, parse_opt, args_doc, doc };

/**
 * Init argument parsing
 */
int8_t arg_parser_init(int argc, char **argv)
{
    int8_t ret = argp_parse (&argp, argc, argv, 0, 0, 0);
    return ret;
}
