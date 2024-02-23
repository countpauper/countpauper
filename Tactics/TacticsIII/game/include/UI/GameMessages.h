#pragma once
#include "UI/Message.h"
#include <cstdint>

namespace Game
{

struct Selected : Engine::Message
{
    Selected(class Avatar* a) : avatar(a) {}
    class Avatar* avatar;
};
}
