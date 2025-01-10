#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "request.h"

#define TOKENS_LENGTH 8

char *METHOD_TOKENS[TOKENS_LENGTH] = {
                                            "OPTIONS", 
                                            "GET",
                                            "HEAD",
                                            "POST",
                                            "PUT",
                                            "DELETE",
                                            "TRACE",
                                            "CONNECT"};

char *parse_request(char *request_line)
{
    for (int i=0; i < TOKENS_LENGTH; i++)
    {
        char *token_ptr = strstr(request_line, METHOD_TOKENS[i]);
        if (token_ptr != NULL)
        {
            char *crlf = strstr(request_line, "\r\n");
            if (crlf != NULL && crlf > token_ptr)
            {
                size_t n  = crlf - token_ptr;
                char *method_string = malloc(n + 1);
                strncpy(method_string, token_ptr, n);
                method_string[n] = '\0';

                return method_string;
            }
        }
        else
        {
            continue;
        }
    }
    return NULL;
}

int validate_request(char *request_message) { 
    /*
    1. if no request return 1
    2. if request received validate it
    3. if request is GET and valid return 0
    4. if request is another method and valid return 2
    5. if request is invalid return -1
    */

    /*
    1. if string_split in METHOD_TOKENS, save to char *method
    - if method is "GET", return 200 ok response
    - if another method, return 500 response
    2. if string_split start with "/", save to char *directory
    3. save rest of the string_split to char *request
    4. check if request is in "HTTP/1.1" format
    */

    char *method = NULL, *uri = NULL, *http_version = NULL;
    char *result = parse_request(request_message);

    if (result != NULL) {
        printf("Request-line received: %s\n", result);

        char *string_split = strtok(result, " ");
        while (string_split != NULL ) {

            if (method == NULL) { 
                int i = 0;
                while (i < TOKENS_LENGTH) {
                    if (strcmp(string_split, METHOD_TOKENS[i]) == 0){
                        method = METHOD_TOKENS[i];
                        printf("METHOD: %s\n", method);
                        break;
                    }
                    i++;
                }
            }

            if (uri == NULL) {
                if (string_split[0] == '/') {
                    uri = string_split;
                    printf("URI: %s\n", uri);
                }
                else {
                    uri = NULL;
                }
            }

            if (http_version == NULL) {
                if (strcmp(string_split, "HTTP/1.1") == 0) {
                    http_version = string_split;
                    printf("HTTP/Version: %s\n", http_version);
                }
            }
            string_split = strtok(NULL, " ");
        }

        printf("Request-line valid\n");
        free(result);
        return 0;
    }
    else {
        printf("No request-line received from client.\n");
        return 1;
    }
}


