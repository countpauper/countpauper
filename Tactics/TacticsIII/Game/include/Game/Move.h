#pragma once
#include "Game/Action.h"
#include "Geometry/Position.h"
#include <vector>
namespace Game
{
class Map;
class Game;

class Move : public Action
{
public:
    Move(Actor& actor, const Game& game, Engine::Position destination, unsigned distance=0);
    void Render() const override;
    void Execute(std::ostream& log) const override;
    unsigned AP() const override;
    std::string Description() const override;
private:
    const Map& map;
    std::vector<Engine::Position> path;
    decltype(path)::const_iterator Reachable() const;
};


}
