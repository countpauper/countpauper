#pragma once
#include "UI/Message.h"
#include <cstdint>

namespace Engine
{

struct Click : Message
{
    Click(int b, int x, int y) : x(x), y(y), button(b) {}
    int x, y;
    int button;
};

struct KeyPressed : Message
{
    KeyPressed(std::uint16_t code, std::uint16_t mods) : key(code), modifiers(mods) {}
    std::uint16_t key;
    std::uint16_t modifiers;
};

struct ClickOn : Message
{
    ClickOn(const class Object& obj, std::uint16_t sub) : object(obj), sub(sub) {}
    const Object& object;
    std::uint16_t sub;
};

struct ScrollWheel : Message
{
    ScrollWheel(bool up) : up(up) {}
    bool up;
};

}
