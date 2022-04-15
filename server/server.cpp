#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <netdb.h>
#include <iostream>
#include "../MessageStructs.h"

int main()
{

    int sockfd;
    char buffer[512];
    // char *hello = "Hello from server";
    struct sockaddr_in servaddr, cliaddr;

    // Creating socket file descriptor
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&servaddr, 0, sizeof(servaddr));
    memset(&cliaddr, 0, sizeof(cliaddr));

    // Filling server information
    servaddr.sin_family = AF_INET; // IPv4
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(8080);

    // Bind the socket with the server address
    if (bind(sockfd, (const struct sockaddr *)&servaddr,
             sizeof(servaddr)) < 0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    unsigned int len;
    int n;

    len = sizeof(cliaddr); // len is value/result
    while (true)
    {
        n = recvfrom(sockfd, (char *)buffer, 512,
                     MSG_WAITALL, (struct sockaddr *)&cliaddr,
                     &len);
        NewClientMessage newClientMsg;
        deserialize((char *)buffer, &newClientMsg);
        std::cout << "New client: " << newClientMsg.name << std::endl;
    }
}