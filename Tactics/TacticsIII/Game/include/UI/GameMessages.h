#pragma once
#include "UI/Message.h"
#include <cstdint>

namespace Game::UI
{

struct Selected : Engine::Message
{
    Selected(class Avatar* a) : avatar(a) {}
    class Avatar* avatar;
};
}
