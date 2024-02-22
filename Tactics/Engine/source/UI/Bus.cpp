#include "UI/Bus.h"

namespace Engine
{

void Bus::Subscribe(Passenger& passenger, std::set<Message::Type> messages)
{
    if (messages.empty())
    {
        subscriptions.erase(&passenger);
    }
    else
    {
        subscriptions[&passenger] = messages;
    }
}

void Bus::Post(const Message& message) const
{
    for(const auto& sub : subscriptions)
    {
        if (sub.second.count(message.GetType()))
        {
            sub.first->OnMessage(message);
        }
    }
}


}
