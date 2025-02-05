#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "utilities.h"

int send_apt_response(char *path);
void send_505_response();
void send_400_response();

int process_response(int cfd, char *request_line)
{
    char *token_ptr;
    char *temp_line = request_line;

    char *method = strtok_r(temp_line, " ", &token_ptr);
    char *uri = strtok_r(NULL, " ", &token_ptr);
    char *http_ver = strtok_r(NULL, " ", &token_ptr);

    if (method == NULL || uri == NULL || http_ver == NULL)
    {
        int code = 400;
        send_400_response();
        printf("%d Bad Request.\n", code);
        return code;
    }

    printf("Method: %s, URI: %s, HTTP Version: %s.\n", method, uri, http_ver);

    if (check_method(method) == 0)
    {
        if (strcmp(method, "GET") != 0)
        {
            int code = 501;
            //send_501_response();
            printf("%d Not Implemented.\n", code);
            return code;
        }
    }
    else 
    {
        int code = 405;
        //send_405_response();
        printf("%d Method Not Allowed.\n", code);
        return code;
    }
    // No need to check uri[0] is backslash, instead check for invalid char.
    //if (uri[0] != '/')
    //{
    //    int code = 400;
    //    send_400_response();
    //    printf("%d Bad Request.\n", code);
    //    return code;
    //}
    if (strcmp(http_ver, "HTTP/1.1") != 0 && strcmp(http_ver, "HTTP/1.0") != 0)
    {
        int code = 505;
        send_505_response();
        printf("%d HTTP Version Not Supported.\n", code);
        return code;
    }

    int code = send_apt_response(uri);
    return code;
}

int send_apt_response(char *path)
{
    const char *root_directory = "server_root";
    if (strcmp(path, "/") == 0)
    {
        const char *requested_file = "/index.html";
        char *full_path = malloc(strlen(root_directory) + strlen(requested_file) + 1);
        strcpy(full_path, root_directory);
        strcat(full_path, requested_file);
        printf("Client requested: %s.\n", full_path);
        free(full_path);
        return 200;
    }
    else
    {
        const char *requested_file = path;
        char *full_path = malloc(strlen(root_directory) + strlen(requested_file) + 1);
        strcpy(full_path, root_directory);
        strcat(full_path, requested_file);
        printf("Client requested: %s.\n", full_path);
        free(full_path);
        return 2;
    }
}


//void send_501_response();
//void send_405_response();


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



char* generate_image_header() {
    char *http_header = "HTTP/1.0 200 OK\r\n"
             "Content-Type: image/jpeg\r\n"
             "Connection: keep-alive\r\n\r\n";
    return http_header;
}

char *greetings_response() {

    FILE *image_file = fopen("server_root/greetings.jpg", "rb");
    if (image_file == NULL) {
        perror("Error opening file");
        return NULL;
    }

    fseek(image_file, 0L, SEEK_END);
    unsigned long file_size = ftell(image_file);
    fseek(image_file, 0L, SEEK_SET);

    char *image_data = (char *)malloc(file_size);

    size_t bytes_read = fread(image_data, 1, file_size, image_file);
    fclose(image_file);
    
    return image_data;

}
