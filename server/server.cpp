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
    char msgBuffer[512];
    // char *hello = "Hello from server";
    struct sockaddr_in servaddr, cliaddr, waitaddr;
    bool waiting = false;
    std::string waitingName;

    // Creating socket file descriptor
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&servaddr, 0, sizeof(servaddr));
    memset(&cliaddr, 0, sizeof(cliaddr));
    memset(&waitaddr, 0, sizeof(waitaddr));

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

        Message *msg = deserialize((char *)buffer);
        switch (msg->type)
        {
        case 0:
        {
            NewClientMessage *newClientMsg = dynamic_cast<NewClientMessage *>(msg);
            std::cout << "New client: " << newClientMsg->name << std::endl;
            if (!waiting)
            {
                waitaddr = cliaddr;
                waiting = true;
                waitingName = newClientMsg->name;
            }
            else
            {
                GameBeginMessage gameBeginMessage;

                gameBeginMessage.opponentName = waitingName;
                int bytesUsed = serialize(&gameBeginMessage, msgBuffer);
                sendto(sockfd, msgBuffer, bytesUsed, 0, (const struct sockaddr *)&cliaddr,
                       sizeof(cliaddr));

                gameBeginMessage.opponentName = newClientMsg->name;
                bytesUsed = serialize(&gameBeginMessage, msgBuffer);
                sendto(sockfd, msgBuffer, bytesUsed, 0, (const struct sockaddr *)&waitaddr,
                       sizeof(waitaddr));

                waiting = false;
                waitingName = "";
            }
            break;
        }
        default:
        {
            break;
        }
        }
        delete msg;
    }
}