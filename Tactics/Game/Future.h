#pragma once

#include <map>
#include "State.h"
#include "IGame.h"

namespace Game
{
	class Action;

	class Future : public IGame
	{
	public:
		Future(const IGame& parent);
		Future(const IGame& parent, const Identity& executor);
		void Adjust(const Identity& actor, const State& state, const std::wstring& description) override;
		void Apply(IGame& root) const;
		State Get(const Identity& actor) const override;
		const Identity* Executor() const;
		bool CanBe(const Position& position) const override;
		bool CanGo(const Position& from, Physics::Direction direction) const override;
		bool Cover(const Position& from, const Position& to) const override;
		std::wstring Description() const override;

		std::vector<std::unique_ptr<Action>> PossibleMoves() const;

		State ActorState() const;
		bool HasParent(const IGame& state) const;
	protected:
		void RecursiveApply(Game& game, std::set<const Actor*>& done) const;
		const IGame& parent;
		const Identity& executor;
		std::map<const Identity*, State> state;
		std::map<const Identity*, std::wstring> description;
	};
} // ::Game