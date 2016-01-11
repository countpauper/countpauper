#pragma once

#include "State.h"
#include "Action.h"

namespace Game
{
    class Game;

 
    class Attack :
        public TargetedAction
    {
    public:
        Attack(Actor& target);
        void Render(const State& state) const override;
        State Act(const State& state, const Game& game) override;
        void React() override;
        struct Properties : public Action::Properties
        {
            Properties(unsigned mp, float range);
            float range;
        };
        Properties& GetAttackProperties();
    private:

    };

 
    class Slash : public Attack
    {
    public:
        Slash(Actor& target);
        Action::Properties& GetProperties() const override;
        static Properties properties;
    };

    class Shoot : public Attack
    {
    public:
        Shoot(Actor& target);
        Action::Properties& GetProperties() const override;
        static Properties properties;
    };

}   // ::Game