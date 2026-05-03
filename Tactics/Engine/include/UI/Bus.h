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
    Bus() = default;
    virtual ~Bus();
    unsigned Subscribe(Passenger& passenger, const std::set<Message::Type>& messages);
    unsigned Unsubscribe(Passenger& passenger, const std::set<Message::Type>& messages);
    unsigned Unsubscribe(Passenger& passenger);
    void Post(const Message& message) const;
private:
    std::map<Passenger*, std::set<Message::Type>> subscriptions;
};

class Passenger
{
public:
    virtual ~Passenger();
    virtual void OnMessage(const Message& message) = 0;
private:
    friend class Bus;
    Bus* bus = nullptr;
};

}
