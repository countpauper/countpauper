#pragma once
#include <vector>
#include "Game/IGame.h"

namespace Game
{
namespace Test
{


class Figment;

class Fantasy : public IGame
{
public:
	Fantasy(); 
	void Imagine(Figment& figment);

	void Adjust(const Identity& actor, const State& state, const std::wstring& description) override;
	void Apply(IGame& root) const;
	State Get(const Identity& actor) const override;
	const Identity* Executor() const;
	bool CanBe(const Position& position) const override;
	bool CanGo(const Position& from, Physics::Direction direction) const override;
	bool Cover(const Position& from, const Position& to) const override;
	std::wstring Description() const override;

	std::vector<Figment*> imagination;
};

}
}
