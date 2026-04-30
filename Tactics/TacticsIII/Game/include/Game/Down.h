#pragma once
#include "Game/Action.h"

namespace Game
{
class World;

class Down : public Action
{
public:
    Down(World& world, Actor& actor);
    void Render() const override;
    Requirements CanDo() const override;
    std::vector<Delta> Execute(std::ostream& log) const override;
    unsigned AP() const override;
    std::string Description() const override;
    Position GetDestination() const override;
    unsigned GetDistance() const override;    
};


}
