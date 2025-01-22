#include <stdio.h>
#include <sys/socket.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <time.h>
#include "utilities.h"

#define REQUEST_MAX_BYTES 1024

void close_socket(int fd)
{
    if (close(fd) == -1)
    {
        perror("[Error] Could not close socket.");
    }
    printf("Socket Closed.\n");
}

void handle_client(int sfd) {

    char *request_buffer = malloc(REQUEST_MAX_BYTES);
    char *method = NULL, *uri = NULL, *http_version = NULL;
    char *token_ptr;
    int validity = 200;
    struct sockaddr_in client_address;
    socklen_t client_addr_size = sizeof(client_address);
    int client_fd;

    while (1) {
        client_fd = accept(sfd, (struct sockaddr*) &client_address, &client_addr_size);
        if (client_fd < 0)
        {
            perror("[Error] Failed to accept connection.");
            continue;
        }
        int byte_count = recv(client_fd, request_buffer, REQUEST_MAX_BYTES, 0);
        char *request_line = strtok_r(request_buffer, "\r\n", &token_ptr);

        if (!request_line) {
            char *http_response = "HTTP/1.1 400 Bad request\r\n\r\n";
            send(client_fd, http_response, strlen(http_response), 0);
            break;
        }

        method = strtok_r(request_line, " ", &token_ptr);
        printf("%s\n", method);
        uri = strtok_r(NULL, " ", &token_ptr);
        printf("%s\n", uri);
        http_version = strtok_r(NULL, " ", &token_ptr);
        printf("%s\n", http_version);

        if ((strcmp(uri, "/") == 0) && (validity == 200)) {
            char *http_response = index_html_response();
            send(client_fd, http_response, strlen(http_response), 0);
            free(http_response);
        }
        else if ((strcmp(uri, "/greetings.jpg") == 0) && (validity == 200)) {
            char *http_header = generate_image_header();
            send(client_fd, http_header, strlen(http_header), 0);

            FILE *image_file = fopen("server_root/greetings.jpg", "rb");
            if (image_file == NULL) {
                perror("Error opening file");
                break;
            }

            fseek(image_file, 0L, SEEK_END);
            unsigned long file_size = ftell(image_file);
            fseek(image_file, 0L, SEEK_SET);

            char *image_data = (char *)malloc(file_size);

            size_t bytes_read = fread(image_data, 1, file_size, image_file);
            send(client_fd, image_data, file_size, 0);
            free(image_data);
            fclose(image_file);
        }
        else {
            char *http_response = "HTTP/1.1 400 Bad request\r\n\r\n";
            send(client_fd, http_response, strlen(http_response), 0);
            break;
        }
    }    
    free(request_buffer);
    close_socket(client_fd);
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        perror("[Error] Please specify IP address and Port for the server");
        return 1;
    }
   
    //char *IP_ADDRESS = argv[1]; 
    int PORT = atoi(argv[1]);

    //printf("IP Address: %s\n", IP_ADDRESS);
    printf("Port: %d\n", PORT);

    int sfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sfd == -1)
    {
        perror("[Error] Failed to open socket.");
        return 1;
    }
    printf("Socket Opened.\n");

    struct sockaddr_in server_address;
    memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;    
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(PORT);

    if (bind(sfd, (const struct sockaddr*) &server_address, sizeof(server_address)) == -1)
    {
        perror("[Error] Failed to bind.");
        close_socket(sfd);
        return 1;
    }
    printf("Socket bound to Port: %d.\n", PORT);
 
    if (listen(sfd, 5) == -1){
        perror("[Error] Failed listening on socket..");
        close_socket(sfd);
    }

    //printf("Connection accepted from %s:%d.\n", inet_ntoa(client_address.sin_addr), ntohs(client_address.sin_port));

    handle_client(sfd);
    
    sleep(5);
    close_socket(sfd);
    return 0;
}











