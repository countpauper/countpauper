#include "Game/Actor.h"


namespace Game 
{

Engine::IntBox Actor::GetBounds() const
{
    return GameBounds(GetPosition(), GetSize());
}

bool Actor::In(const Engine::IntBox& bounds) const
{
    return bool(GetBounds() & bounds);
}



}