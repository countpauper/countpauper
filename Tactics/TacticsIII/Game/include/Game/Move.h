#pragma once
#include "Game/Action.h"
#include "Geometry/Position.h"
#include <vector>
namespace Game
{
class HeightMap;
class World;

class Move : public Action
{
public:
    Move(Actor& actor, const World& world, Engine::Position destination, unsigned distance=0);
    void Render() const override;
    bool CanDo() const override;
    void Execute(std::ostream& log) const override;
    unsigned AP() const override;
    std::string Description() const override;
private:
    const HeightMap& map;
    std::vector<Engine::Position> path;
    decltype(path)::const_iterator Reachable() const;
};


}
