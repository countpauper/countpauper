#pragma once
#include "Game/Action.h"

namespace Game
{

class Attack : public Action
{
public:
    Attack(Actor& actor, Actor& target);
    void Render() const override;
    bool CanDo() const override;
    void Execute(std::ostream& log) const override;
    unsigned AP() const override;
    std::string Description() const override;
private:
    Actor& target;
};


}
