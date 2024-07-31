#include "Game/Attack.h"
#include "Geometry/Line.h"
#include "Utility/Random.h"
#include "UI/Avatar.h"
#include <GL/gl.h>

namespace Game
{

Attack::Attack(Avatar& actor, Avatar& target) :
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

void Attack::Execute(std::ostream& log) const
{
    actor.GetCounts().Cost(Stat::ap, AP());
    auto chance = actor.HitChance(target);
    if (chance < Engine::Random().Chance())
    {
        log << actor.Name() << "misses " << target.Name(); // TODO: this hit chance should be specified in miss, block, parry obstacle
        return;
    }

    auto damage =  actor.GetStats().Get(Stat::offense) - target.GetStats().Get(Stat::defense);
    if ( damage.Total() > 0 )
    {
        target.GetCounts().Cost(Stat::hp, damage.Total(), true);
        log << actor.Name() << " deals " << target.Name() << " " << damage.Description() << " Damage" << std::endl;
    }
    else
    {
        log << actor.Name() << " deals " << target.Name() << " a glancing blow" << std::endl;
    }
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
