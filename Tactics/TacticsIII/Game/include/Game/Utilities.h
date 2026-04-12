#pragma once
#include <string>
#include "Game/Position.h"
#include "Game/Orientation.h"
#include "Game/Statted.h"

namespace Game
{

class Actor;
class World;

Engine::Orientations Facing(Position from, Position to);
Engine::Range<float> AttackHeight(const World& world, const Actor& from, const Actor& to);
unsigned AttackDistance(Position delta);
unsigned AttackDistance(const Actor& from, const Actor& to);
float VerticalReach(Position delta, unsigned reach);
Engine::Range<float> VerticalReach(const Actor& from, Position to);
double HitChance(const Actor& actor, const Actor& target);
Computation ComputeDamage(const Computations& offense, const Computations& defense);

}
