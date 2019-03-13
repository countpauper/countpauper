#pragma once

#include <map>
#include <set>
#include <memory>
#include <vector>
#include "Position.h"
#include "Direction.h"
#include "Damage.h"
#include "Body.h"
#include "Stats.h"
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
        State(const Body& body, Position pos=Position(0,0), Direction dir=Direction::North, unsigned mp=10,
            std::vector<const Armor*> armor = {}, std::vector<const Weapon*> weapons = {}, Actor::Knowledge knowledge = {});

        Position position;
        Direction direction;
        unsigned mp;
        unsigned loyalty;
        Body body;
        std::vector<const Armor*> worn;
        std::vector<const Weapon*> carried;
        Actor::Knowledge knowledge;

        bool IsPossible(const Skill& skill, const State& target) const;
        Damage AttackDamage(const Skill& skill) const;
        Damage AttackDamage(const Skill& skill, const Score& skillLevel) const;
        bool Hurt(const Body::Part& part, const Damage& damage, const std::wstring& description);
        Score Chance(const Skill& skill, const State& target) const;
        Score Chance(const Skill& skill) const;

        Score Strength() const;
        Score Agility() const;
        Score Constitution() const;
        Score Intelligence() const;
        Score Wisdom() const;
        Score AttributeScore(Attribute attribute) const;
        Anatomy Origin(const Skill& skill, Trajectory trajectory) const;

        Score SkillLevel(const Skill& skill, const State* victim = nullptr) const;
        Score Range(const Skill& skill) const;
        void Engage(const Skill& skill);
    private:
        Bonus StrengthBonus() const;
        Bonus Encumberance() const;
        Bonus ConstitutionBonus() const;
        Bonus IntelligenceBonus() const;
        Bonus WisdomBonus() const;
        Bonus Charge() const;   // excess enchantment
        Score AttributeScore(const Body::Part& limb, Attribute attribute) const;
        Damage Mitigation(const Body::Part& location) const;
        Score ArmorBonus(const Skill& skill) const;
        Score FullBodyBonus(Attribute attribute) const;
        Score FreeLimbScore(const Weapon& weapon, Attribute attribute) const;
        Score AttributeBonus(Attribute attribute) const;
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