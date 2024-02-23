#pragma once
#include <set>
#include <map>
#include "UI/Message.h"

namespace Engine
{

class Passenger;

class Bus
{
public:
    void Subscribe(Passenger& passenger, std::set<Message::Type> messages);
    void Post(const Message& message) const;
private:
    std::map<Passenger*, std::set<Message::Type>> subscriptions;
};

class Passenger
{
public:
    virtual void OnMessage(const Message& message) = 0;
};

}
