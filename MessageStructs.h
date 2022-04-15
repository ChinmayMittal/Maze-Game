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

struct GameBeginMessage : Message
{
    GameBeginMessage()
    {
        type = 1;
    }
    std::string opponentName;
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
    {
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
    case 1:
    {
        GameBeginMessage *msgGameBegin = dynamic_cast<GameBeginMessage *>(msg);
        char *p = (char *)q;
        for (int i = 0; i < msgGameBegin->opponentName.length(); i++)
        {
            *p = msgGameBegin->opponentName[i];
            p++;
            bytesUsed++;
        }
        *p = '\0';
        p++;
        bytesUsed++;
        break;
    }
    }
    return bytesUsed;
}

Message *deserialize(char *data)
{
    int *q = (int *)data;
    int type = *q;
    q++;
    switch (type)
    {
    case 0:
    {
        NewClientMessage *msgNewClient = new NewClientMessage();
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
        return msgNewClient;
    }
    case 1:
    {
        GameBeginMessage *msgGameBegin = new GameBeginMessage();
        char *p = (char *)q;
        int nameLen = strlen(p);
        for (int i = 0; i < nameLen; i++)
        {
            msgGameBegin->opponentName += *p;
            p++;
        }
        return msgGameBegin;
    }
    default:
    {
        return NULL;
    }
    }
}

#endif