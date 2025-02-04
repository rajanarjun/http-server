#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "utilities.h"

int process_response(int cfd, char *request_line)
{
    char *token_ptr;
    char *temp = request_line;

    char *method = strtok_r(temp, " ", &token_ptr);
    char *url = strtok_r(NULL, " ", &token_ptr);
    char *http_ver = strtok_r(NULL, " ", &token_ptr);

    printf("Method: %s, URL: %s, HTTP Version: %s.\n", method, url, http_ver);
    return 200;
}



char *index_html_response() {

    const char *http_header = "HTTP/1.0 200 OK\r\n"
                              "Content-Type: text/html\r\n"
                              "Content-Length: %lu\r\n"
                              "Connection: close\r\n\r\n";

    FILE *index_html = fopen("server_root/index.html", "r");
    if (index_html == NULL) {
        perror("Error opening file");
        return NULL;
    }

    fseek(index_html, 0L, SEEK_END);
    unsigned long file_size = ftell(index_html);
    fseek(index_html, 0L, SEEK_SET);

    size_t total_response_size = file_size + strlen(http_header) + 1;
    char *http_response = (char *)malloc(total_response_size);

    sprintf(http_response, http_header, file_size);

    size_t bytes_read = fread(http_response + strlen(http_header), 1,file_size, index_html);
    fclose(index_html);
    
    return http_response;

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
