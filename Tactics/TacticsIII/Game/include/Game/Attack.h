#pragma once
#include "Game/Action.h"
namespace Game
{



class Attack : public Action
{
public:
    using Targets=std::tuple<std::reference_wrapper<Actor>>;   
    Attack(World& world, Actor& actor, std::reference_wrapper<Actor> target);
    
    void Render() const override;
    Requirements CanDo() const override;
    std::vector<Delta> Execute(std::ostream& log) const override;
    unsigned AP() const override;
    std::string Description() const override;
    Position GetDestination() const override;
    unsigned GetDistance() const override;    
private:
    Actor& target;
};


}
