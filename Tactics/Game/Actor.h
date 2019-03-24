#pragma once

#include <vector>
#include <set>
#include "Engine/Random.h"
#include "Object.h"
#include "Body.h"
#include "Stats.h"
#include "Score.h"
#include "Item.h"
#include "Skills.h"

namespace Game
{
    class State;
    class Action;
    class Plan;
    
	class Actor : public Object
	{
    public:
        Actor();
        void Render() const override;
		unsigned GetMovePoints() const;
        Direction GetDirection() const;
        void Apply(const State& result) override;
        bool IsAlly(const Actor& other) const;
        void Turn() override;
        void Activate(const Game& game) override;
        void Deactivate();
        bool Trigger(const Actor& actor, Game& game);
        bool IsActive() const;
        bool IsIdle() const;    // active, but no plan
        bool IsAnticipating() const; 
        
        void AI(Game& game);
        void Execute(Game& game);
        unsigned GetTeam() const;
        bool CanAct() const;
        bool Dead() const;
        bool Prone() const override;
        using Knowledge = std::set<const Skill*> ;
        const Skill* DefaultAttack() const;
        const Skill* DefaultMove() const;
        const Skill* WaitSkill() const;

        std::set<const Skill*> Counters(const Skill& previous) const;
        std::set<const Skill*> Combos(const Skill& previous) const;

        bool IsSelectable(const Skill& skill) const;
        const Knowledge& GetSkills() const;
        int MovePoints() const;

        std::set<const Armor*> Worn() const;
        std::set<const Weapon*> Carried() const;
        std::map<const Part*, const Weapon*> Wielded() const;
        std::unique_ptr<Plan> plan;
    private:
        Bonus AgilityMoveBonus() const;
        Score GetMaxMovePoints() const;

        friend std::wistream& operator>>(std::wistream& s, Actor& actor);
        bool active;
        unsigned mp;
        unsigned team;
        Direction direction;
		Anatomy anatomy;
		Knowledge knowledge;
        std::vector<std::unique_ptr<Armor>> worn;
        std::vector<std::unique_ptr<Weapon>> carried;
    };
    std::wistream& operator>>(std::wistream& s, Actor& actor);

}   // ::Game

