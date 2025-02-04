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



