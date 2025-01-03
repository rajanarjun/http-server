#include <stdio.h>
#include <sys/socket.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        printf("Please specify IP Adress and Port for the server\n");
        return 1;
    }
    
    char *IP_ADDRESS =argv[1];
    int PORT = atoi(argv[2]);

    printf("IP Address: %s\n", IP_ADDRESS);
    printf("Port: %d\n", PORT);

    int sfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sfd == -1)
    {
        printf("Failed to open socket. Error code: %d\n", errno);
        return 1;
    }

    struct sockaddr_in server_address;
    memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = inet_addr(IP_ADDRESS);
    server_address.sin_port = htons(PORT);
    
    close(sfd);
    return 0;
}
