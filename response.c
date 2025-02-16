#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "utilities.h"

#define TOKENS_LENGTH 8

int send_apt_response(char *path);
void send_error_response(int error_code);

char *METHOD_TOKENS[TOKENS_LENGTH] = {"OPTIONS", 
                                      "GET",
                                      "HEAD",
                                      "POST",
                                      "PUT",
                                      "DELETE",
                                      "TRACE",
                                      "CONNECT"};

//To check if http method is valid
int check_method(char *method)
{
    for (int i = 0; i < TOKENS_LENGTH; i++)
    {
        if (strcmp(method, METHOD_TOKENS[i]) == 0)
        {
            return 0;
        }
    }
    return 1;
}

void process_response(int cfd, char *request_line)
{
    int code;
    char *token_ptr;
    char *temp_line = request_line;

    char *method = strtok_r(temp_line, " ", &token_ptr);
    char *uri = strtok_r(NULL, " ", &token_ptr);
    char *http_ver = strtok_r(NULL, " ", &token_ptr);

    // tokens could be null
    if (method == NULL || uri == NULL || http_ver == NULL)
    {
        code = 400;
        send_error_response(code);
        printf("%d Bad Request.\n", code);
    }

    printf("Method: %s, URI: %s, HTTP Version: %s.\n", method, uri, http_ver);

    // check if method if valid
    if (check_method(method) == 0)
    {
        if (strcmp(method, "GET") != 0)
        {
            code = 501;
            send_error_response(code);
            printf("%d Not Implemented.\n", code);
        }
    }
    else 
    {
        code = 405;
        send_error_response(code);
        printf("%d Method Not Allowed.\n", code);
    }
    
    // checking for http version, anything apart from 1.0 and 1.1 is 505
    if (strcmp(http_ver, "HTTP/1.0") != 0 && strcmp(http_ver, "HTTP/1.1") != 0)
    {
        code = 505;
        send_error_response(code);
        printf("%d HTTP Version Not Supported.\n", code);
    }
    
    // Dont need this block as '/' is added by default by browsers
    // but fuck it i am still checking it.
    if (uri[0] != '/')
    {
        code = 400;
        send_error_response(code);
        printf("%d Bad Request: invalid start to path.\n", code);
    }

    // checking for dangerous path traversal. 
    if (strstr(uri, "../") != NULL)
    {
        code = 400;
        send_error_response(code);
        printf("%d Bad Request: dangerous path traversal.\n", code);
    }

    // fml.
    // checking for control ,non visible characters, and percent-encoding
    for (int i = 0; uri[i] != '\0'; i++)
    {
        if ( uri[i] == '\\' || ( (uri[i] == '%') && (!isxdigit(uri[i+1]) || !isxdigit(uri[i+2])) ) )
        {
            code = 400;
            send_error_response(code);
            printf("%d Bad Request: invalid character or percend-encoding in URI.\n", code);
        }
    }
    
    send_ok_response(uri);
}

void send_error_response(int error_code)
{
    const char *error_directory = "server_root/error/";
    const char *requested_file;
    const char *http_header;

    switch (error_code)
    {
        case 400:
            requested_file = "400.html";
            http_header = "HTTP/1.0 400 Bad Request.\r\n"
                              "Content-Type: text/html\r\n"
                              "Connection: close\r\n\r\n";
            break;
        case 404:
            requested_file = "404.html"; 
            http_header = "HTTP/1.0 404 Not Found.\r\n"
                              "Content-Type: text/html\r\n"
                              "Connection: close\r\n\r\n";
            break;
        case 405:
            requested_file = "405.html"; 
            http_header = "HTTP/1.0 405 Method Not Allowed.\r\n"
                              "Content-Type: text/html\r\n"
                              "Connection: close\r\n\r\n";
            break;
        case 501:
            requested_file = "501.html"; 
            http_header = "HTTP/1.0 501 Not Implemened.\r\n"
                              "Content-Type: text/html\r\n"
                              "Connection: close\r\n\r\n";
            break;
        case 505:
            requested_file = "505.html"; 
            http_header = "HTTP/1.0 505 HTTP Version Not Supported.\r\n"
                              "Content-Type: text/html\r\n"
                              "Connection: close\r\n\r\n";
            break;
    }
    
    char *full_path = malloc(strlen(error_directory) + strlen(requested_file) + 1);
    strcpy(full_path, error_directory);
    strcat(full_path, requested_file);
    printf("Client requested: %s.\n", full_path);
    free(full_path);
}

void send_ok_response(char *path)
{
    const char *root_directory = "server_root";
    char *http_header = "HTTP/1.0 200 OK.\r\n"
                  "Content-Type: %s\r\n"
                  "Connection: close\r\n\r\n";
    const char *requested_file;

    requested_file = (strcmp(path, "/") == 0) ?  "/index.html" : path;

    char *full_path = malloc(strlen(root_directory) + strlen(requested_file) + 1);
    strcpy(full_path, root_directory);
    strcat(full_path, requested_file);
    printf("Client requested: %s.\n", full_path);

    file_mime_type = get_mime_type(requested_file);


    file_data = read_file(full_path);
    if (file_data == NULL) 
    {
        send_error_response(404);
    }
    else
    {
        // TODO:serve header and file
    }

    free(full_path);
    free(file_data);
}


