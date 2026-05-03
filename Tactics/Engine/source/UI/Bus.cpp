#include "UI/Bus.h"
#include "Utility/Assert.h"
#include <algorithm>

namespace Engine
{

Bus::~Bus()
{
    for(auto& sub : subscriptions)
    {
        sub.first->bus = nullptr;
    }
}

unsigned Bus::Subscribe(Passenger& passenger, const std::set<Message::Type>& messages)
{
    auto [it, inserted] = subscriptions.insert(std::make_pair(&passenger, messages));
    if (inserted)
    {
        assert(!passenger.bus); // resubscription or multiple subscription not supported yet
        passenger.bus = this;
        return messages.size();
    }
    it->second.insert(messages.begin(), messages.end());
    return messages.size(); // TODO only should be new ones, in fact return the set to unsub scoped
}

unsigned Bus::Unsubscribe(Passenger& passenger, const std::set<Message::Type>& messages)
{
    auto it = subscriptions.find(&passenger);
    if (it == subscriptions.end())
        return 0;
    std::set<Message::Type> difference;
    std::set_difference(it->second.begin(), it->second.end(),
        messages.begin(), messages.end(),
        std::inserter(difference, difference.begin()));
    it->second = difference;
    return messages.size(); // TODO should only be ones that were in there, in fact return set
}

unsigned Bus::Unsubscribe(Passenger& passenger)
{
    auto it = subscriptions.find(&passenger);
    if (it == subscriptions.end())
        return 0;
    unsigned current = it->second.size();
    subscriptions.erase(it);
    passenger.bus = nullptr;
    return current;
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

Passenger::~Passenger()
{
    if (bus)
        bus->Unsubscribe(*this);
}
}
