#include <stdio.h>
#include <string.h>
#include <stdlib.h>

char *generate_response() {

    const char *http_header = "HTTP/1.0 200 OK\r\n""Content-Type: text/html\r\n\r\n";

    FILE *index_html = fopen("server_root/index.html", "r");
    if (index_html == NULL) {
        perror("Error opening file");
        return NULL;
    }

    fseek(index_html, 0L, SEEK_END);
    unsigned long file_size = ftell(index_html);
    fseek(index_html, 0L, SEEK_SET);
   
    size_t total_response_size = strlen(http_header) + file_size + 3;
    char *http_response = (char *)malloc(total_response_size);
    char *file_buffer = (char *)malloc(file_size);
    
    strcpy(http_response, http_header);

    size_t bytes_read = fread(file_buffer, sizeof(char), file_size, index_html);
    fclose(index_html);
    
    strcat(http_response, file_buffer);
    strcat(http_response, "\r\n");

    free(file_buffer);
    return http_response;

}
