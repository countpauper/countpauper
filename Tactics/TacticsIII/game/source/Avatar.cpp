#include "UI/Avatar.h"

namespace Game
{

Avatar::Avatar(Creature& c) :
    creature(c)
{

}

std::string_view Avatar::Name() const
{
    return creature.Name();
}

}
