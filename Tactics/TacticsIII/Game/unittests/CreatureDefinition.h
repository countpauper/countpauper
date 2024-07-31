#include "Game/StatDefinition.h"
#include "Game/Stat.h"
namespace Game::Test
{

class CreatureDefinition
{
public:
    CreatureDefinition();
    CreatureDefinition& Define(Stat::Id stat, Stat::Id dependency=Stat::Id::none, float multiplier=1.0);
    CreatureDefinition& Define(Stat::Id stat, Stat::Id dependency, Stat::Operator op, Stat::Id operand);
    CreatureDefinition& Define(Stat::Id stat, int value);
    CreatureDefinition& Count();
private:
    void Ensure(Stat::Id id);
    Stat::Id last;
};

}
