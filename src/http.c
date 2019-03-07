//
//  i2c_hw.c
//  sps30
//
//  Created by Benjamin Rannow on 02.02.19.
//  Copyright © 2019 Benjamin Rannow. All rights reserved.
//

#include <stdio.h>
#include <curl/curl.h>
#include <string.h>
#include <stdlib.h>

int8_t http_send_post_request(const char *url, const char *postName, const char *data)
{
    CURL *curl;
    CURLcode res;
    
    /* In windows, this will init the winsock stuff */
    curl_global_init(CURL_GLOBAL_ALL);
    
    /* get a curl handle */
    curl = curl_easy_init();
    if(curl) {
        uint32_t dataLength = strlen(data);
        uint32_t outputLength = strlen(postName) + dataLength + 2; // +1 for = and the other +1 is for \0
        char *output = (char *)calloc(outputLength , sizeof(char));
        strcpy(output, postName);
        strncat(output, "=", 1);
        strncat(output, data, dataLength);
        strncat(output, "\0", 1);
        
        curl_easy_setopt(curl, CURLOPT_URL, url);
        
        curl_easy_setopt(curl, CURLOPT_POST, 1);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, output);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0);
        
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, 1);
        curl_easy_setopt(curl, CURLOPT_HEADER, 0);
        curl_easy_setopt(curl, CURLOPT_FORBID_REUSE, 1);
        curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 1);
        curl_easy_setopt(curl, CURLOPT_DNS_CACHE_TIMEOUT, 10);
        curl_easy_setopt(curl, CURLOPT_FRESH_CONNECT, 1);
        
        /* Perform the request, res will get the return code */
        res = curl_easy_perform(curl);
        /* Check for errors */
        if(res != CURLE_OK) {
            fprintf(stderr, "curl_easy_perform() failed: %s\n",
                    curl_easy_strerror(res));
        }
        
        /* always cleanup */
        curl_easy_cleanup(curl);
        free(output);
    }
    curl_global_cleanup();
    if (res != CURLE_OK) {
        return -res;
    }
    
    return 0;
}
