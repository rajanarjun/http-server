#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "utilities.h"

#define TOKENS_LENGTH 8

char *METHOD_TOKENS[TOKENS_LENGTH] = {"OPTIONS", 
                                      "GET",
                                      "HEAD",
                                      "POST",
                                      "PUT",
                                      "DELETE",
                                      "TRACE",
                                      "CONNECT"};

int check_method(const char *method) {
    for (int i = 0; i < TOKENS_LENGTH; i++) {
        if (strcmp(method, METHOD_TOKENS[i]) == 0) {
            return 0;
        }
    }
    return 1;
}

int validate_request(char *request_message) { 

    char *method = NULL, *uri = NULL, *http_version = NULL;
    char *token_ptr;

    char *request_line = strtok_r(request_message, "\r\n", &token_ptr);
    if (!request_line) {
        printf("[Error] No request-line received.\n");
        return 400;
    }

    method = strtok_r(request_line, " ", &token_ptr);
    uri = strtok_r(NULL, " ", &token_ptr);
    http_version = strtok_r(NULL, " ", &token_ptr);

    if (method == NULL || uri == NULL || http_version == NULL) {
        printf("[Error] Missing components in request-line.\n");
        return 400;
    }

    if (check_method(method) == 1) {
        printf("[Error] Unsupported method in request-line: '%s'.\n", method);
        return 400;
    }

    if (uri[0] != '/') {
        printf("[Error] URI must start with '/'.\n");
        return 400;
    }

    if (strcmp(http_version, "HTTP/1.1") != 0 && strcmp(http_version, "HTTP/1.0") != 0) {
        printf("[Error] Unsupported HTTP version '%s'.\n", http_version);
        return 400;
    }

    if (strcmp(method, "GET") == 0) {
        printf("Valid GET request.\n");
        return 200; 
    } 
    else {
        printf("Valid request but unsupported method: %s.\n", method);
        return 501;
    }
}


