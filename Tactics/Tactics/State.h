#pragma once

#include <map>
#include <set>
#include <memory>
#include <vector>
#include "Game/Position.h"
#include "Game/Direction.h"
#include "Game/Damage.h"
#include "Game/Body.h"
#include "Game/Stats.h"
#include "Item.h"
#include "IGame.h"
#include "Actor.h"

namespace Game
{
    class Actor;
    class Game;
    class Action;
    class Skill;
    class Anatomy;

    class State
    {
    public:
        State(const Actor& actor);

        bool IsPossible(const Skill& skill, const State& target) const;

        Position position;
        Direction direction;
        unsigned mp;
        Body body;
        Damage AttackDamage(const Skill& skill) const;
        Damage Mitigation(const Body::Part& location) const;
        Score Chance(const Skill& skill) const;
        Score Strength() const;
        Score Agility() const;
        Score Constitution() const;
        Score Intelligence() const;
        Score Wisdom() const;
        Score SkillLevel(const Skill& skill) const;
        Score Range(const Skill& skill) const;
        unsigned loyalty;
        bool Prone() const;
        void Fall();
    private:
        Bonus StrengthBonus() const;
        Bonus Encumberance() const;
        Bonus ConstitutionBonus() const;
        Bonus IntelligenceBonus() const;
        Bonus WisdomBonus() const;
        Bonus Charge() const;   // excess enchantment
        const Weapon* MatchWeapon(const Skill& skill) const;

        std::vector<const Armor*> worn;
        std::vector<const Weapon*> wielded;
        std::vector<Actor::Know> knowledge;
    };

    class GameState : public IGame
    {
    public:
        GameState(const IGame& parent);
        GameState(const IGame& parent, const Actor& executor);
        void Adjust(const Actor& actor, const State& state) override;
        void Apply(IGame& root) const;
        State Get(const Actor& actor) const override;

        const Actor* Executor() const;
        bool CanBe(const Position& position) const override;
        bool CanGo(const Position& from, Direction direction) const override;
        bool Cover(const Position& from, const Position& to) const override;
        std::wstring Description() const override;

        State ActorState() const;
        bool HasParent(const IGame& state) const;
    protected:
        void RecursiveApply(Game& game, std::set<const Actor*>& done) const;
        const IGame& parent;
        const Actor& executor;
        std::map<const Actor*, State> state;
    };

    using GameChances= std::vector<std::pair<double, GameState*>>;
} // ::Game