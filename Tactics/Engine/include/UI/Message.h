#pragma once
#include <typeindex>

namespace Engine
{

struct Message
{
    using Type = std::type_index;
    virtual ~Message() = default;
    virtual Type GetType() const { return Type(typeid(*this)); }
};

template<typename T>
constexpr Message::Type MessageType() { return std::type_index(typeid(T)); }

}
