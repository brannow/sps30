//
//  arg_parser.c
//  sps30
//
//  Created by Benjamin Rannow on 14.02.19.
//  Copyright © 2019 Benjamin Rannow. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <argp.h>
#include <string.h>

#include "arg_parser.h"


#define PATH_MAX_LENGTH 255
#define MAX_SCANNING_TIME_SECONDS 255
#define POST_NAME_MAX_LENGTH 30

const char *argp_program_version = "sps30 0.1";
const char *argp_program_bug_address = "<benjamin@fuyukai.moe>";

/* Program documentation. */
static char doc[] = "SPS30 I2C Interface with output functionality into file or direct sending to a server";

/* A description of the arguments we accept. */
static char args_doc[] = "";

struct argp_option options[] =
{
    {"time",            't', "SECONDS",     0, "sample rate timing. Max: 255, Default: 1."},
    
    {"avg",             'a', 0,             0, "shrink all collected data since last output to one set."},
    {"json",            'j', 0,             0, "shrink all collected data since last output to one set."},
    
    {"file",            'f', "PATH",        0, "Writes data into a file (overwrite). (max 250 chars)"},
    {"file-append",     'p', 0,             0, "Append data into file (only relevant for file option)."},
    {"url",             'u', "URL",         0, "Send Data to given URL."},
    {"url-post-name",   'n', "NAME",        0, "The URL POST name (max 30 chars). Default: 'data'."},
    {0}
};

static error_t parse_opt (int key, char *arg, struct argp_state *state)
{
    error_t err = 0;
    struct arg_parser_arguments *arguments = (struct arg_parser_arguments*)(state->input);
    
    switch (key) {
        case 't': {
            int32_t time = arg ? atoi (arg) : 0;
            if (time > 0 && time <= MAX_SCANNING_TIME_SECONDS) {
                arguments->time = (uint8_t)time;
            } else if (time > MAX_SCANNING_TIME_SECONDS) {
                arguments->time = MAX_SCANNING_TIME_SECONDS;
                printf("WARNING: max time is %d; time auto set to %d\n", MAX_SCANNING_TIME_SECONDS);
            }
            break;
        }
        case 'a': {
            arguments->avg = 1;
            break;
        }
        case 'j': {
            arguments->json = 1;
            break;
        }
        case 'f': {
            arguments->file = (char *)malloc(sizeof(char) * PATH_MAX_LENGTH + 1);
            strncpy(arguments->file, arg, PATH_MAX_LENGTH);
            arguments->file[PATH_MAX_LENGTH] = '\0';
            if (strlen(arg) > PATH_MAX_LENGTH) {
                printf("WARNING: file path max length is %d\n", PATH_MAX_LENGTH);
            }
            break;
        }
        case 'p': {
            arguments->file_append = 1;
            break;
        }
        case 'u': {
            arguments->url = (char *)malloc(sizeof(char) * PATH_MAX_LENGTH + 1);
            strncpy(arguments->url, arg, PATH_MAX_LENGTH);
            arguments->url[PATH_MAX_LENGTH] = '\0';
            if (strlen(arg) > PATH_MAX_LENGTH) {
                printf("WARNING: url max length is %d\n", PATH_MAX_LENGTH);
            }
            break;
        }
        case 'n': {
            arguments->url_post_name = (char *)malloc(sizeof(char) * POST_NAME_MAX_LENGTH + 1);
            strncpy(arguments->url_post_name, arg, POST_NAME_MAX_LENGTH);
            arguments->url_post_name[POST_NAME_MAX_LENGTH] = '\0';
            if (strlen(arg) > POST_NAME_MAX_LENGTH) {
                printf("WARNING: url-post-name max length is %d\n", POST_NAME_MAX_LENGTH);
            }
            break;
        }
        default: {
            err = ARGP_ERR_UNKNOWN;
            break;
        }
    }
    
    return err;
}

/* Our argp parser. */
static struct argp argp = { options, parse_opt, args_doc, doc };

/**
 * Init argument parsing
 */
int8_t arg_parser_init(int argc, char **argv, struct arg_parser_arguments *arguments)
{
    arguments->avg = 0;
    arguments->json = 0;
    arguments->file_append = 0;
    arguments->time = 0;
    arguments->file = NULL;
    arguments->url = NULL;
    arguments->url_post_name = NULL;
    
    int8_t ret = argp_parse(&argp, argc, argv, 0, 0, arguments);
    return ret;
}
