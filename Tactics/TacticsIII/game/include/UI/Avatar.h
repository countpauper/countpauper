#pragma once
#include "UI/Object.h"
#include "Game/Creature.h"

namespace Game
{

class Avatar :
        public Engine::Object
{
public:
    explicit Avatar(Creature& c);
    std::string_view Name() const override;
private:
    Creature& creature;
};

}
