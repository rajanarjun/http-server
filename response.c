#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <sys/socket.h>
#include "utilities.h"

#define TOKENS_LENGTH 8
#define HEADER_MAX_SIZE 8192

void send_ok_response(int cfd, char *path);
void send_error_response(int cfd, int error_code);

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

// checking consecutive backslash
int check_slashes(const char *str) 
{
    int slash_count = 0;  
    while (*str) 
    {
        if (*str == '/') 
        {
            if (slash_count) 
            {
                //printf("Send 400, consecutive slashes found.\n");
                return 1;
            }
            slash_count = 1;
        } 
        else 
        {
            slash_count = 0;
        }
        // move pointer to next char
        str++;
    }
    return 0;
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
        send_error_response(cfd, code);
        printf("%d Bad Request.\n", code);
    }

    printf("Method: %s, URI: %s, HTTP Version: %s.\n", method, uri, http_ver);

    // check if method if valid
    if (check_method(method) == 0)
    {
        if (strcmp(method, "GET") != 0)
        {
            code = 501;
            send_error_response(cfd, code);
            printf("%d Not Implemented.\n", code);
        }
    }
    else 
    {
        code = 405;
        send_error_response(cfd, code);
        printf("%d Method Not Allowed.\n", code);
    }
    
    // checking for http version, anything apart from 1.0 and 1.1 is 505
    if (strcmp(http_ver, "HTTP/1.0") != 0 && strcmp(http_ver, "HTTP/1.1") != 0)
    {
        code = 505;
        send_error_response(cfd, code);
        printf("%d HTTP Version Not Supported.\n", code);
    }
    
    // Dont need this block as '/' is added by default by browsers
    // but fuck it i am still checking it.
    if (uri[0] != '/')
    {
        code = 400;
        send_error_response(cfd, code);
        printf("%d Bad Request: invalid start to path.\n", code);
    }

    // checking for dangerous path traversal. 
    if (strstr(uri, "../") != NULL)
    {
        code = 400;
        send_error_response(cfd, code);
        printf("%d Bad Request: dangerous path traversal.\n", code);
    }

    if (check_slashes(uri) == 1)
    {
        code = 400;
        send_error_response(cfd, code);
        printf("%d Bad Request: consecutive slashes in path.\n", code);
    }

    // fml.
    // checking for control ,non visible characters, and percent-encoding
    for (int i = 0; uri[i] != '\0'; i++)
    {
        if ( uri[i] == '\\' || ( (uri[i] == '%') && (!isxdigit(uri[i+1]) || !isxdigit(uri[i+2])) ) )
        {
            code = 400;
            send_error_response(cfd, code);
            printf("%d Bad Request: invalid character or percend-encoding in URI.\n", code);
        }
    }
    
    send_ok_response(cfd, uri);
}

void send_error_response(int cfd, int error_code)
{
    const char *error_directory = "server_root/error_pages/";
    char *requested_file;
    char http_header[HEADER_MAX_SIZE];
    char *temp_header;

    switch (error_code)
    {
        case 400:
            requested_file = "400.html";
            temp_header = "HTTP/1.0 400 Bad Request.\r\n"
                              "Content-Type: %s\r\n"
                              "Content-Length: %d\r\n"
                              "Connection: close\r\n\r\n";
            break;
        case 404:
            requested_file = "404.html"; 
            temp_header = "HTTP/1.0 404 Not Found.\r\n"
                              "Content-Type: %s\r\n"
                              "Content-Length: %d\r\n"
                              "Connection: close\r\n\r\n";
            break;
        case 405:
            requested_file = "405.html"; 
            temp_header = "HTTP/1.0 405 Method Not Allowed.\r\n"
                              "Content-Type: %s\r\n"
                              "Content-Length: %d\r\n"
                              "Connection: close\r\n\r\n";
            break;
        case 501:
            requested_file = "501.html"; 
            temp_header = "HTTP/1.0 501 Not Implemened.\r\n"
                              "Content-Type: %s\r\n"
                              "Content-Length: %d\r\n"
                              "Connection: close\r\n\r\n";
            break;
        case 505:
            requested_file = "505.html"; 
            temp_header = "HTTP/1.0 505 HTTP Version Not Supported.\r\n"
                              "Content-Type: %s\r\n"
                              "Content-Length: %d\r\n"
                              "Connection: close\r\n\r\n";
            break;
    }
    
    char *full_path = malloc(strlen(error_directory) + strlen(requested_file) + 1);
    strcpy(full_path, error_directory);
    strcat(full_path, requested_file);

    FILE *file_to_send = open_file(full_path);
    if (file_to_send == NULL) 
    {
        free(full_path);
        perror("[Error] Custom error page not found\n");
    }
    else
    {
        printf("Client requested: %s\n", full_path);
        char *file_mime_type = get_mime_type(requested_file);
        unsigned long file_size = get_file_size(file_to_send);

        sprintf(http_header, temp_header,
                  file_mime_type,
                  file_size);

        /* printf("%s\n", http_header); */

        // send the header with content type and length
        send(cfd, http_header, strlen(http_header), 0);
        
        // send the file
        char *f_data = get_file_data(file_to_send, file_size);
        send(cfd, f_data, file_size, 0);

        free(full_path);
        free(f_data);
        close_file(file_to_send);
    }
}

void send_ok_response(int cfd, char *path)
{
    const char *root_directory = "server_root";
    char *requested_file = (strcmp(path, "/") == 0) ?  "/index.html" : path;
    char http_header[HEADER_MAX_SIZE];

    char *full_path = malloc(strlen(root_directory) + strlen(requested_file) + 1);
    strcpy(full_path, root_directory);
    strcat(full_path, requested_file);

    printf("Client requested: %s\n", full_path);

    FILE *file_to_send = open_file(full_path);
    if (file_to_send == NULL) 
    {
        free(full_path);
        send_error_response(cfd, 404);
    }
    else
    {
        char *file_mime_type = get_mime_type(requested_file);
        unsigned long file_size = get_file_size(file_to_send);

        sprintf(http_header,
                "HTTP/1.0 200 OK.\r\n"
                  "Content-Type: %s\r\n"
                  "Content-Length: %lu\r\n"
                  "Connection: close\r\n\r\n",
                  file_mime_type,
                  file_size);
    
        // send the header with content type and length
        send(cfd, http_header, strlen(http_header), 0);
        
        // send the file
        char *f_data = get_file_data(file_to_send, file_size);
        send(cfd, f_data, file_size, 0);

        free(full_path);
        free(f_data);
        close_file(file_to_send);
    }
}


