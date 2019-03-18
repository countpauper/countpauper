#pragma once
#include <string>
#include "Game/Identity.h"
#include "Game/State.h"

namespace Game
{
namespace Test
{


class Figment : public State, public Identity
{
public:
	Figment(const std::wstring& description, const Anatomy& anatomy, Position pos = Position(0, 0), Direction dir = Direction::North, unsigned mp = 10,
		std::vector<const Armor*> armor = {}, std::vector<const Weapon*> weapons = {}, Actor::Knowledge knowledge = {});
	void Apply(const State& state) override;
	std::wstring Description() const override;
private:
	std::wstring description;
};

}}