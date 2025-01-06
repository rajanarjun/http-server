#include <stdio.h>
#include <sys/socket.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <time.h>

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
    if (argc != 3)
    {
        perror("[Error] Please specify IP address and Port for the server");
        return 1;
    }
   
    char *IP_ADDRESS = argv[1]; 
    int PORT = atoi(argv[2]);

    printf("IP Address: %s\n", IP_ADDRESS);
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
    server_address.sin_port = htons(PORT);

    if (inet_pton(AF_INET, IP_ADDRESS, &server_address.sin_addr) <= 0)
    {
        perror("[Error] inet_pton failed");
        close_socket(sfd);
        return 1;
    }

    if (bind(sfd, (const struct sockaddr*) &server_address, sizeof(server_address)) == -1)
    {
        perror("[Error] Failed to bind.");
        close_socket(sfd);
        return 1;
    }
    printf("Socket bound to IP address: %s, Port: %d.\n", IP_ADDRESS, PORT);
 
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

    char *http_response = 
    "HTTP/1.0 200 OK\r\n"
    "Content-Type: text/html\r\n"
    "Connection: close\r\n"
    "\r\n"
    "hello from arjun\n";

    send(cfd, http_response, strlen(http_response), 0);

    close_socket(sfd);
    return 0;
}












