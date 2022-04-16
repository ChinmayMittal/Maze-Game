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

struct GameUpdateMessage : Message
{
    GameUpdateMessage()
    {
        type = 2;
    }
    int x, y, velX, velY, moveFactor, money, points;
    float health;
};

struct GameEndMessage : Message
{
    GameEndMessage()
    {
        type = 3;
    }
    int points;
    int money;
    float health;
};

struct GameResultMessage : Message
{
    GameResultMessage()
    {
        type = 4;
    }
    bool won;
};

int serialize(Message *msg, char *data);

Message *deserialize(char *data);

#endif