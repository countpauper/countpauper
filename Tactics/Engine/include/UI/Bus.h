#pragma once
#include <set>
#include <map>
#include <typeindex>
#include <cstdint>

namespace Engine
{

class Passenger;

struct Message
{
    using Type = std::type_index;
    virtual ~Message() = default;
    virtual Type GetType() const { return Type(typeid(*this)); }
};

struct Click : Message
{
    Click(int x, int y, int b) : x(x), y(y), button(b) {}
    int x, y;
    int button;
};

struct Key : Message
{
    Key(std::uint16_t code) : key(code) {}
    std::uint16_t key;
};




template<typename T>
constexpr Message::Type MessageType() { return std::type_index(typeid(T)); }

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
