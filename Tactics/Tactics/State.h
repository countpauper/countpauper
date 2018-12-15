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

namespace Game
{
    class Actor;
    class Game;
    class Action;
    class Skill;

    class State
    {
    public:
        State(const Actor& actor);

        bool IsPossible(const Skill& skill, const State& target) const;

        Position position;
        Direction direction;
        unsigned mp;
        Body body;
        Damage AttackDamage() const;
        Damage Mitigation() const;
        Score DefendChance() const;
        Score Strength() const;
        Score Agility() const;
        Score Constitution() const;
        Score Intelligence() const;
        Score Wisdom() const;
        unsigned loyalty;
    private:
        Bonus StrengthBonus() const;
        Score StrReqPenalty() const;
        Bonus ConstitutionBonus() const;
        Bonus IntelligenceBonus() const;
        Bonus WisdomBonus() const;
        Score WisReqPenalty() const;

        std::vector<const Armor*> worn;
        std::vector<const Weapon*> wielded;
    };

    class GameState : public IGame
    {
    public:
        GameState();
        GameState(const IGame& parent);
        void Adjust(const Actor& actor, const State& state) override;
        void Apply(IGame& root) const;
        State Get(const Actor& actor) const override;

        Actor* ActiveActor() const override;
        bool CanBe(const Position& position) const override;
        bool CanGo(const Position& from, Direction direction) const override;
        bool Cover(const Position& from, const Position& to) const override;
        std::wstring Description() const override;

        State ActorState() const;
    protected:
        void RecursiveApply(Game& game, std::set<const Actor*>& done) const;
        const IGame& parent;
        std::map<const Actor*, State> state;
    };
    /*
    class GameChance
    {
    public:
        GameChance(IGame& state, double chance, const std::wstring& description);
        GameChance(GameChance&& other);
        GameChance& operator=(GameChance&& other);
        GameChance(const GameChance& other) = delete;
        GameChance& operator=(const GameChance& other) = delete;
        
        bool operator<(const GameChance& other) const;
        bool operator>(const GameChance& other) const;
        double chance;
        std::wstring description;
        GameState* operator->() const;
        GameState& operator*() const;
    protected:
        std::unique_ptr<GameState> state;
    };
    */
    using GameChances= std::vector<std::pair<double, GameState*>>;
} // ::Game