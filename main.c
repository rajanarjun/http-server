#include <stdio.h>
#include <sys/socket.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <time.h>
#include "request.h"

#define REQUEST_MAX_BYTES 1024

void close_socket(int fd)
{
    if (close(fd) == -1)
    {
        perror("[Error] Could not close socket.");
    }
    printf("Socket Closed.\n");
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

    struct sockaddr_in client_address;
    socklen_t client_addr_size = sizeof(client_address);
    int cfd = accept(sfd, (struct sockaddr*) &client_address, &client_addr_size);
    if (cfd == -1)
    {
        perror("[Error] Failed to accept connection.");
        close_socket(sfd);
        return 1;
    }
    
    printf("Connection accepted from %s:%d.\n", inet_ntoa(client_address.sin_addr), ntohs(client_address.sin_port));

    char *request_buffer = malloc(REQUEST_MAX_BYTES);
    memset(request_buffer, 0, REQUEST_MAX_BYTES);
    int byte_count = recv(cfd, request_buffer, REQUEST_MAX_BYTES, 0);

    //printf("Received %d bytes of data in buf\n", byte_count);
    //printf("%s", request_buffer);

    parse_request(request_buffer);

    // send response only when correct http get request is made:
    //char *http_response = 
    //"HTTP/1.0 200 OK\r\n"
    //"Content-Type: text/html\r\n"
    //"Connection: close\r\n"
    //"\r\n"
    //"Hello from PASYBLO047\n";

    //send(cfd, http_response, strlen(http_response), 0);

    free(request_buffer);
    close_socket(sfd);
    return 0;
}












