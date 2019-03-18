#include <assert.h>
#include "Figment.h"
#include "Game/Damage.h"

namespace Game
{
namespace Test
{

Figment::Figment(const std::wstring& description, const Anatomy& anatomy, Position pos, Direction dir, unsigned mp,
	std::vector<const Armor*> armor, std::vector<const Weapon*> weapons, Actor::Knowledge knowledge) :
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

std::wstring Figment::Description() const
{
	return description;
}
}
}

