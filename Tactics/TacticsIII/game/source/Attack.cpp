#include "Game/Attack.h"
#include "Geometry/Line.h"
#include "UI/Avatar.h"
#include <GL/gl.h>

namespace Game
{

Attack::Attack(Avatar& actor, const Game& game, Avatar& target) :
    Action(actor),
    target(target)
{
}

void Attack::Render() const
{
    glColor3d(1.0, 1.0, 0.0);
    Engine::Line line(actor.GetCoordinate(), target.GetCoordinate());
    line += Engine::Vector::Z;
    line.Render();
}

void Attack::Execute() const
{
    target.GetCreature().Cost(Stat::hp, target.GetCreature().Get(Stat::offense));
    actor.GetCreature().Cost(Stat::ap, AP());
}

unsigned Attack::AP() const
{
    return 1;
}

std::string Attack::Description() const
{
    return std::string("Attack ") + std::string(target.Name());
}

}
