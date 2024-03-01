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
    KeyPressed(std::uint8_t code, unsigned char ascii, std::uint16_t mods) : code(code), ascii(ascii), modifiers(mods) {}
    std::uint8_t code;
    unsigned char ascii;
    std::uint16_t modifiers;
};

struct ClickOn : Message
{
    ClickOn(const class Object* obj, std::uint16_t sub) : object(obj), sub(sub) {}
    const Object* object;
    std::uint16_t sub;
};

struct ScrollWheel : Message
{
    ScrollWheel(bool up) : up(up) {}
    bool up;
};

struct Redraw : Message
{
    Redraw(class Object* obj=nullptr) : object(obj) {}
    class Object* object;
};

}
