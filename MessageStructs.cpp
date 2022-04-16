#include "MessageStructs.h"

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
    case 2:
    {
        GameUpdateMessage *msgGameUpdate = dynamic_cast<GameUpdateMessage *>(msg);
        *q = msgGameUpdate->x;
        q++;
        *q = msgGameUpdate->y;
        q++;
        *q = msgGameUpdate->velX;
        q++;
        *q = msgGameUpdate->velY;
        q++;
        *q = msgGameUpdate->moveFactor;
        q++;
        *q = msgGameUpdate->money;
        q++;
        *q = msgGameUpdate->points;
        q++;
        bytesUsed += 7 * sizeof(int);
        float *p = (float *)q;
        *q = msgGameUpdate->health;
        q++;
        bytesUsed += sizeof(float);
        char *r = (char *)p;
        *r = '\0';
        r++;
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
    case 2:
    {
        GameUpdateMessage *msgGameUpdate = new GameUpdateMessage();
        msgGameUpdate->x = *q;
        q++;
        msgGameUpdate->y = *q;
        q++;
        msgGameUpdate->velX = *q;
        q++;
        msgGameUpdate->velY = *q;
        q++;
        msgGameUpdate->moveFactor = *q;
        q++;
        msgGameUpdate->money = *q;
        q++;
        msgGameUpdate->points = *q;
        q++;
        float *p = (float *)q;
        msgGameUpdate->health = *p;
        p++;
        return msgGameUpdate;
    }
    default:
    {
        return NULL;
    }
    }
}