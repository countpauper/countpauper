#pragma once
#include "Game/Action.h"
#include "Geometry/Position.h"
#include <vector>
namespace Game
{
class World;

class Move : public Action
{
public:
    Move(World& world, Actor& actor, Position destination, unsigned distance=0);
    void Render() const override;
    Requirements CanDo() const override;
    std::vector<Delta> Execute(std::ostream& log) const override;
    unsigned AP() const override;
    std::string Description() const override;
    Position GetDestination() const override;
    unsigned GetDistance() const override;    
private:
    Position destination;
    unsigned distance;
    std::vector<Position> path;
    decltype(path)::const_iterator Reachable() const;
};


}
