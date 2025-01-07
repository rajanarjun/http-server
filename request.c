#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "request.h"

const char *METHOD_TOKEN = "GET";

void parse_request(char *request_line)
{
    char *token_ptr = strstr(request_line, METHOD_TOKEN);
    char *crlf = strstr(request_line, "\r\n");
    if (token_ptr != NULL && crlf != NULL && crlf > token_ptr)
    {
        size_t n  = crlf - token_ptr;
        char *method_string = malloc(n + 1);
        strncpy(method_string, token_ptr, n);
        method_string[n] = '\0';

        printf("%s\n", method_string);

        free(method_string);
    }

}

//int validate_request()
//{
//    // if request valid return 0, else -1 
//    int validity 
//
//}


