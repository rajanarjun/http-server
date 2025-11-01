#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <sys/socket.h>
#include "response.h"
#include "file_info.h"

#define TOKENS_LENGTH 8
#define HEADER_MAX_SIZE 8192

char http_header[HEADER_MAX_SIZE];

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

int check_other_stuff(const char *str) {
    for (int i = 0; str[i] != '\0'; i++)
    {
        if (str[i] == '\\' || ((str[i] == '%') && (!isxdigit(str[i+1]) || !isxdigit(str[i+2])))) {
            return 1;
        }
    }
    return 0;
}

void send_error_response(int cfd, int error_code)
{
    FileInfo file;
    const char *error_directory = "../server_root/error_pages/";
    char *requested_file;
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
            temp_header = "HTTP/1.0 501 Not Implemented.\r\n"
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
    
    if (load_file(requested_file, error_directory, &file) != 0) {
        // fallback header if error pages itself fail
        char fallback_header[256];
        const char *fallback_body = "<html><body><h1>500 Internal Server Error</h1></body></html>";
        sprintf(fallback_header, "HTTP/1.0 500 Internal Server Error\r\n"
                                    "Content-Type: text/html\r\n"
                                    "Content-Length: %zu\r\n"
                                    "Connection: close\r\n\r\n",
                                    strlen(fallback_body));
        // send fallback header
        send(cfd, fallback_header, strlen(fallback_header), 0);
        // send fallback body
        send(cfd, fallback_body, strlen(fallback_body), 0);

    } else {
        printf("Client requested: %s\n", file.path);

        sprintf(http_header, temp_header, file.mime_type, file.size);

        // send the header with content type and length
        send(cfd, http_header, strlen(http_header), 0);

        // send the file
        send(cfd, file.data, file.size, 0);

        free_file(&file);
    }
}

void send_ok_response(int cfd, char *path)
{
    FileInfo file;
    const char *root_directory = "../server_root";
    char *requested_file = (strcmp(path, "/") == 0) ?  "/index.html" : path;

    if (load_file(requested_file, root_directory, &file) != 0) {
        send_error_response(cfd, 404);

    } else {
        printf("Client requested: %s\n", file.path);

        sprintf(http_header,"HTTP/1.0 200 OK.\r\n"
                            "Content-Type: %s\r\n"
                            "Content-Length: %lu\r\n"
                            "Connection: close\r\n\r\n",
                            file.mime_type,
                            file.size);

        // send the header with content type and length
        send(cfd, http_header, strlen(http_header), 0);

        // send the file
        send(cfd, file.data, file.size, 0);
        
        free_file(&file);
    }
}


void process_response(int cfd, const char *request_line)
{
    char *token_ptr;
    char *temp_line = malloc(strlen(request_line) + 1);
    strcpy(temp_line, request_line);

    char *method = strtok_r(temp_line, " ", &token_ptr);
    char *uri = strtok_r(NULL, " ", &token_ptr);
    char *http_ver = strtok_r(NULL, " ", &token_ptr);

    // tokens could be null
    if (method == NULL || uri == NULL || http_ver == NULL)
    {
        printf("Bad Request.\n");
        send_error_response(cfd, 400);
        free(temp_line);
        return;
    }
    // check if method is valid
    if (check_method(method) == 1)
    {
        printf("Method Not Allowed.\n");
        send_error_response(cfd, 405);
        free(temp_line);
        return;
    }
    // check if method is GET, other methods not implemented yet
    if (strcmp(method, "GET") != 0)
    {
        printf("Not Implemented.\n");
        send_error_response(cfd, 501);
        free(temp_line);
        return;
    }
    // checking for http version, anything apart from 1.0 and 1.1 is 505
    if (strcmp(http_ver, "HTTP/1.0") != 0 && strcmp(http_ver, "HTTP/1.1") != 0)
    {
        printf("HTTP Version Not Supported.\n");
        send_error_response(cfd, 505);
        free(temp_line);
        return;
    }
    // Dont really need this block as '/' is added by default by browsers, but still
    if (uri[0] != '/')
    {
        printf("Bad Request.\n");
        send_error_response(cfd, 400);
        free(temp_line);
        return;
    }
    // checking for path traversal. 
    if (strstr(uri, "../") != NULL)
    {
        printf("Bad Request.\n");
        send_error_response(cfd, 400);
        free(temp_line);
        return;
    }
    // checking for weird slashes or consecutive ones
    if (check_slashes(uri) == 1)
    {
        printf("Bad Request.\n");
        send_error_response(cfd, 400);
        free(temp_line);
        return;
    }
    // checking for control ,non visible characters, and percent-encoding
    if (check_other_stuff(uri) == 1)
    {
        printf("Bad Request.\n");
        send_error_response(cfd, 400);
        free(temp_line);
        return;
    }
    // all is good
    printf("Method: %s, URI: %s, HTTP Version: %s.\n", method, uri, http_ver);
    send_ok_response(cfd, uri);
    free(temp_line);
}

