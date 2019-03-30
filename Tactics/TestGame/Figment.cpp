#include <assert.h>
#include "Figment.h"
#include "Game/Damage.h"

namespace Game
{
namespace Test
{

Figment::Figment(const std::wstring& description, const Anatomy& anatomy, Position pos, Direction dir, unsigned mp,
	std::set<const Armor*> armor, std::set<const Weapon*> weapons, Actor::Knowledge knowledge) :
	State(anatomy, pos, dir, mp, armor, weapons, knowledge),
	description(description)
{

}

void Figment::Apply(const State& state)
{

	assert(this != &state);	// applied to self is not right

	position = state.position;
	direction = state.direction;
	mp = state.mp;
	body = state.body;
}

void Figment::Event(const std::wstring& event)
{
	events.emplace_back(event);
}

bool Figment::HasEvent(const std::wstring& part) const
{
	return std::any_of(events.begin(), events.end(), [&part](const decltype(events)::value_type& event)
	{	// regex would be nice here
		return event.find(part, 0) != std::wstring::npos;
	});
}

std::wstring Figment::Description() const
{
	return description;
}

Position Figment::GetPosition() const
{
	return position;
}

}
}

