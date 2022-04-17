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
#include <map>
#include "../MessageStructs.h"

int main()
{

    int sockfd;
    char buffer[512];
    char msgBuffer[512];
    // char *hello = "Hello from server";
    struct sockaddr_in servaddr, cliaddr, waitaddr, otherAddr;
    bool waiting = false;
    std::string waitingName;

    bool p1Ended = false;
    bool p2Ended = false;
    int p1Points = 0;
    int p2Points = 0;

    std::map<in_addr_t, in_addr_t> opponents;

    // Creating socket file descriptor
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&servaddr, 0, sizeof(servaddr));
    memset(&cliaddr, 0, sizeof(cliaddr));
    memset(&waitaddr, 0, sizeof(waitaddr));
    memset(&otherAddr, 0, sizeof(otherAddr));

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

            std::cout << "Client IP: " << inet_ntoa(cliaddr.sin_addr) << std::endl;
            if (!waiting)
            {
                waitaddr = cliaddr;
                waiting = true;
                waitingName = newClientMsg->name;
            }
            else
            {
                otherAddr = cliaddr;

                GameBeginMessage gameBeginMessage;

                gameBeginMessage.opponentName = waitingName;
                int bytesUsed = serialize(&gameBeginMessage, msgBuffer);
                sendto(sockfd, msgBuffer, bytesUsed, 0, (const struct sockaddr *)&cliaddr,
                       sizeof(cliaddr));
                // std::cout << "Sending to " << cliaddr.sin_addr.s_addr << std::endl;
                gameBeginMessage.opponentName = newClientMsg->name;
                bytesUsed = serialize(&gameBeginMessage, msgBuffer);
                sendto(sockfd, msgBuffer, bytesUsed, 0, (const struct sockaddr *)&waitaddr,
                       sizeof(waitaddr));
                // std::cout << "Sending to " << waitaddr.sin_addr.s_addr << std::endl;
                opponents.insert(std::pair<in_addr_t, in_addr_t>(cliaddr.sin_addr.s_addr, waitaddr.sin_addr.s_addr));
                opponents.insert(std::pair<in_addr_t, in_addr_t>(waitaddr.sin_addr.s_addr, cliaddr.sin_addr.s_addr));

                waiting = false;
                waitingName = "";
            }
            break;
        }
        case 2:
        {
            // auto itr = opponents.find(cliaddr.sin_addr.s_addr);
            //  std::cout << "Received from " << inet_ntoa(cliaddr.sin_addr) << std::endl;

            // if (itr != opponents.end())
            // {
            // struct sockaddr_in sendAddr;
            // memset(&sendAddr, '\0', sizeof(sendAddr));
            // // std::cout << "Iterator first: " << itr->first << " second: " << itr->second << std::endl;
            // sendAddr.sin_addr.s_addr = itr->second;
            // sendAddr.sin_family = servaddr.sin_family = AF_INET;
            // sendAddr.sin_port = htons(8080);

            int e;
            if (cliaddr.sin_addr.s_addr == waitaddr.sin_addr.s_addr)
            {
                e = sendto(sockfd, buffer, n, 0, (const struct sockaddr *)&otherAddr,
                           sizeof(otherAddr));
            }
            else
            {
                e = sendto(sockfd, buffer, n, 0, (const struct sockaddr *)&waitaddr,
                           sizeof(waitaddr));
            }

            // std::cout << e << std::endl;
            if (e == -1)
            {
                fprintf(stderr, "socket() failed: %s\n", strerror(errno));
            }
            // std::cout << "Sent to " << inet_ntoa(cliaddr.sin_addr) << std::endl;
            //}
            break;
        }
        case 3:
        {
            GameEndMessage *gameEndMsg = dynamic_cast<GameEndMessage *>(msg);
            if (cliaddr.sin_addr.s_addr == waitaddr.sin_addr.s_addr)
            {
                p1Ended = true;
                p1Points = gameEndMsg->points;
                std::cout << "P1 Points " << p1Points << std::endl;
            }
            else
            {
                p2Ended = true;
                p2Points = gameEndMsg->points;
                std::cout << "P2 Points " << p2Points << std::endl;
            }
        }
        default:
        {
            break;
        }
        }
        delete msg;
        if (p1Ended && p2Ended)
        {
            GameResultMessage gameResP1, gameResP2;
            gameResP1.won = p1Points >= p2Points;
            int numBytes = serialize(&gameResP1, msgBuffer);
            sendto(sockfd, msgBuffer, numBytes, 0, (const struct sockaddr *)&waitaddr,
                   sizeof(waitaddr));
            gameResP2.won = p2Points >= p1Points;
            numBytes = serialize(&gameResP2, msgBuffer);
            sendto(sockfd, msgBuffer, numBytes, 0, (const struct sockaddr *)&otherAddr,
                   sizeof(otherAddr));
            p1Ended = false;
            p2Ended = false;
            waiting = false;
        }
    }
}