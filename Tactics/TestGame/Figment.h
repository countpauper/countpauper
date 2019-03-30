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
	Figment(const std::wstring& description, const Anatomy& anatomy, Position pos = Position(0, 0), Direction dir = Direction::east, unsigned mp = 10,
		std::set<const Armor*> armor = {}, std::set<const Weapon*> weapons = {}, Actor::Knowledge knowledge = {});
	void Apply(const State& state) override;
	std::wstring Description() const override;
	Position GetPosition() const override;
	void Event(const std::wstring& description);
	bool HasEvent(const std::wstring& part) const;
	std::vector<std::wstring> events;
private:
	std::wstring description;
};

}}