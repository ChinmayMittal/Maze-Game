#ifndef MESSAGE_STRUCTS_H
#define MESSAGE_STRUCTS_H

#include <string>
#include <string.h>
#include <iostream>

struct Message
{
    int type;
    virtual ~Message() {}
};

struct NewClientMessage : Message
{
    NewClientMessage()
    {
        type = 0;
    }
    std::string name;
};

int serialize(Message *msg, char *data)
{
    int bytesUsed = 0;
    int *q = (int *)data;
    *q = msg->type;
    q++;
    bytesUsed += sizeof(int);
    switch (msg->type)
    {
    case 0:
        NewClientMessage *msgNewClient = dynamic_cast<NewClientMessage *>(msg);
        char *p = (char *)q;
        for (int i = 0; i < msgNewClient->name.length(); i++)
        {
            *p = msgNewClient->name[i];
            p++;
            bytesUsed++;
        }
        *p = '\0';
        p++;
        bytesUsed++;
        break;
    }
    return bytesUsed;
}

void deserialize(char *data, Message *msg)
{
    int *q = (int *)data;
    msg->type = *q;
    q++;
    switch (msg->type)
    {
    case 0:
        NewClientMessage *msgNewClient = dynamic_cast<NewClientMessage *>(msg);
        char *p = (char *)q;
        // int j = 0;
        // while (p[j] != 0)
        // {
        //     std::cout << p[j];
        //     j++;
        // }
        int nameLen = strlen(p);
        for (int i = 0; i < nameLen; i++)
        {
            msgNewClient->name += *p;
            p++;
        }
        break;
    }
}

#endif