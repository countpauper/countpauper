#pragma once
#include <string>
#include "Game/Position.h"
#include "Game/Orientation.h"
#include "Game/Statted.h"

namespace Game
{

class Actor;
class World;

Orientations Facing(Position from, Position& to);
double ComputeCover(const World& world, const Actor& from, const Actor& to);
double HitChance(const Actor& actor, const Actor& target);
Computation ComputeDamage(const Computations& offense, const Computations& defense);

}
