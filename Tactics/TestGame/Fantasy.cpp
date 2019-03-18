#include <cassert>
#include "Fantasy.h"
#include "Figment.h"

namespace Game
{
namespace Test
{
Fantasy::Fantasy() = default;


void Fantasy::Imagine(Figment& figment)
{
	imagination.push_back(&figment);
}

void Fantasy::Adjust(const Identity& id, const State& state, const std::wstring& description)
{
	assert(std::find(imagination.begin(), imagination.end(), &id) != imagination.end());
	const_cast<::Game::Identity&>(id).Apply(state);
}

void Fantasy::Apply(IGame& root) const
{
	assert(&root == this);
}

State Fantasy::Get(const Identity& figment) const
{
	return dynamic_cast<const State&>(figment);
}

const Identity* Fantasy::Executor() const
{
	if (imagination.empty())
		return nullptr;
	else
		return imagination.front();
}

bool Fantasy::CanBe(const Position& position) const
{
	for (const auto image : imagination)
	{
		if (image->GetPosition() == position)
			return false;
	}
	return true;
}

bool Fantasy::CanGo(const Position& from, Direction direction) const
{
	return true;
}
bool Fantasy::Cover(const Position& from, const Position& to) const
{
	return false;
}

std::wstring Fantasy::Description() const
{
	return L"Fantasy";
}

}
}