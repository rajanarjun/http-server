#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "utilities.h"

#define TOKENS_LENGTH 8

int check_method(char *method);
int send_apt_response(char *path);
void send_505_response();
void send_400_response();

char *METHOD_TOKENS[TOKENS_LENGTH] = {"OPTIONS", 
                                      "GET",
                                      "HEAD",
                                      "POST",
                                      "PUT",
                                      "DELETE",
                                      "TRACE",
                                      "CONNECT"};

int process_response(int cfd, char *request_line)
{
    int code;
    char *token_ptr;
    char *temp_line = request_line;

    char *method = strtok_r(temp_line, " ", &token_ptr);
    char *uri = strtok_r(NULL, " ", &token_ptr);
    char *http_ver = strtok_r(NULL, " ", &token_ptr);

    //Forgot to check if these tokens could be null, and got segfault when testing,
    //so added this check.
    if (method == NULL || uri == NULL || http_ver == NULL)
    {
        code = 400;
        printf("%d Bad Request.\n", code);
    }

    printf("Method: %s, URI: %s, HTTP Version: %s.\n", method, uri, http_ver);

    if (check_method(method) == 0)
    {
        if (strcmp(method, "GET") != 0)
        {
            code = 501;
            printf("%d Not Implemented.\n", code);
        }
    }
    else 
    {
        code = 405;
        printf("%d Method Not Allowed.\n", code);
    }
    
    //checking for http version, anything apart from 1.0 and 1.1 is 505
    if (strcmp(http_ver, "HTTP/1.0") != 0 && strcmp(http_ver, "HTTP/1.1") != 0)
    {
        code = 505;
        printf("%d HTTP Version Not Supported.\n", code);
    }
    
    //Dont need this block as '/' is added by default by browsers
    //but fuck it i am still checking it.
    if (uri[0] != '/')
    {
        code = 400;
        printf("%d Bad Request: invalid start to path.\n", code);
    }

    //OH MY GOD THIS IS HELLL
    //Checking for control and non visible characters, backslashes and percent-encoding
    //FML, FUCK RFC
    for (int i = 0; uri[i] != '\0'; i++)
    {
        if ( uri[i] == '\\' || !isprint(uri[i]) || ( (uri[i] == '%') && (!isxdigit(uri[i+1]) || !isxdigit(uri[i+2])) ) )
        {
            code = 400;
            printf("%d Bad Request: invalid character or percend-encoding in URI.\n", code);
        }
    }
    
    send_apt_response(code, uri);
    return code
    
}

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

void send_apt_response(int status_code, char *path)
{
    const char *error_directory = "server_root/error";
    const char *requested_file;

    switch (status_code)
    {
        case 400:
            break;
        case 404:
            break;
        case 405:
            break;
        case 501:
            break;
        case 505:
            break;
    }


    //ternary op syntax = condition ? expression_if_true : expression_if_false;
    requested_file = (strcmp(path, "/") == 0) ?  "/index.html" : path;

    char *full_path = malloc(strlen(root_directory) + strlen(requested_file) + 1);

    strcpy(full_path, root_directory);
    strcat(full_path, requested_file);
    printf("Client requested: %s.\n", full_path);

    // TODO:
    // search for that file in server_root, if not present serve 404 else next step
    // get file mime type from requested_file and generate http header and serve file, 200 OK

    free(full_path);
    // return could be 404 if file was not found
    return 200;
}

//TODO:
//Yeh saare error response ko ek hi function mei dalna hai, switch case use kar sakte hai
//Need to actually send the file over socket
void send_400_response()
{
    const char *http_header = "HTTP/1.0 400 Bad Request.\r\n"
                              "Content-Type: text/html\r\n"
                              "Connection: close\r\n\r\n";

    FILE *html_file = fopen("server_root/error/400.html", "r");
    if (html_file == NULL) {
        perror("Error opening file");
    }

    fseek(html_file, 0L, SEEK_END);
    unsigned long file_size = ftell(html_file);
    fseek(html_file, 0L, SEEK_SET);

    size_t total_response_size = file_size + strlen(http_header) + 1;
    char *http_response = malloc(total_response_size);

    strcpy(http_response, http_header);

    size_t bytes_read = fread(http_response, 1, file_size, html_file);
    printf("%s\n.",http_response);

    fclose(html_file);
    free(http_response);
}

//TODO:
//Need to actually send the file over socket
void send_505_response()
{
    const char *http_header = "HTTP/1.0 505 HTTP Version Not Supported.\r\n"
                              "Content-Type: text/html\r\n"
                              "Connection: close\r\n\r\n";

    FILE *html_file = fopen("server_root/error/505.html", "r");
    if (html_file == NULL) {
        perror("Error opening file");
    }

    fseek(html_file, 0L, SEEK_END);
    unsigned long file_size = ftell(html_file);
    fseek(html_file, 0L, SEEK_SET);

    size_t total_response_size = file_size + strlen(http_header) + 1;
    char *http_response = malloc(total_response_size);

    strcpy(http_response, http_header);

    size_t bytes_read = fread(http_response, 1, file_size, html_file);
    printf("%s\n.",http_response);

    fclose(html_file);
    free(http_response);
}

//char* generate_image_header() {
//    char *http_header = "HTTP/1.0 200 OK\r\n"
//             "Content-Type: image/jpeg\r\n"
//             "Connection: keep-alive\r\n\r\n";
//    return http_header;
//}
//
//char *greetings_response() {
//
//    FILE *image_file = fopen("server_root/greetings.jpg", "rb");
//    if (image_file == NULL) {
//        perror("Error opening file");
//        return NULL;
//    }
//
//    fseek(image_file, 0L, SEEK_END);
//    unsigned long file_size = ftell(image_file);
//    fseek(image_file, 0L, SEEK_SET);
//
//    char *image_data = (char *)malloc(file_size);
//
//    size_t bytes_read = fread(image_data, 1, file_size, image_file);
//    fclose(image_file);
//    
//    return image_data;
//
//}
