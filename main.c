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

void close_client_socket(int cfd)
{
    if (close(cfd) == -1)
    {
        perror("[Error] Could not close client socket.");
    }
    printf("Client socket closed.\n");
}

void close_server_socket(int sfd)
{
    if (close(sfd) == -1)
    {
        perror("[Error] Could not close server socket.");
    }
    printf("Server socket closed.\n");
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        perror("[Error] Please enter port for the server");
        return 1;
    }
   
    int PORT = atoi(argv[1]);
    printf("Port: %d\n", PORT);

    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1)
    {
        perror("[Error] Failed to open socket.");
        return 2;
    }
    printf("Socket Opened.\n");

    struct sockaddr_in server_address;
    memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;    
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(PORT);

    if (bind(server_fd, (const struct sockaddr*) &server_address, sizeof(server_address)) == -1)
    {
        perror("[Error] Failed to bind.");
        close_server_socket(server_fd);
        return 3;
    }
    printf("Socket bound to Port: %d.\n", PORT);
 
    if (listen(server_fd, 5) == -1)
    {
        perror("[Error] Failed listening on socket..");
        close_server_socket(server_fd);
        return 4;
    }

    struct sockaddr_in client_address;
    socklen_t client_address_size = sizeof(client_address);

    while (1)
    {
        int client_fd = accept(server_fd, (struct sockaddr*) &client_address, &client_address_size);
        if (client_fd == -1)
        {
            perror("[Error] Failed to accept connection.");
            break;
        }
        printf("Accepted connection from address: %s.\n", inet_ntoa(client_address.sin_addr));

        char message[REQUEST_MAX_BYTES];
        size_t message_bytes = recv(client_fd, message, REQUEST_MAX_BYTES, 0);
        if (message_bytes < 0)
        {
            perror("[Error] 0 bytes received from client.\n");
            close_client_socket(client_fd);
            break;
        }

        char *temp = message;
        char *req = strtok_r(temp, "\r\n", &temp);
        printf("Request Received: %s\n", req);

        process_response(client_fd, req);
    }

    sleep(2);
    close_server_socket(server_fd);
    return 0;
}

