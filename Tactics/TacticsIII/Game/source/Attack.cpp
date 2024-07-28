#include "Game/Attack.h"
#include "Geometry/Line.h"
#include "Utility/Random.h"
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

void Attack::Execute(std::ostream& log) const
{
    actor.GetCreature().Cost(Stat::ap, AP());
    auto chance = actor.HitChance(target);
    if (chance < Engine::Random().Chance())
    {
        log << actor.Name() << "misses " << target.Name(); // TODO: this hit chance should be specified in miss, block, parry obstacle
        return;
    }

    auto damage =  actor.GetCreature().Get(Stat::offense) - target.GetCreature().Get(Stat::defense);
    if ( damage.Total() > 0 )
    {
        auto& targetCreature = target.GetCreature();
        targetCreature.Damage(damage.Total());
        if (targetCreature.IsKO())
        {
            log << actor.Name() << " knocks " << target.Name() << " out with " << damage.Description() << " Damage" << std::endl;
        }
        else
        {
            log << actor.Name() << " deals " << target.Name() << " " << damage.Description() << " Damage" << std::endl;
        }
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
