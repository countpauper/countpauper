#include "Game/StatDefinition.h"
#include "Game/Stat.h"

namespace Game::Test
{

class Definition
{
public:
    Definition(StatDefinition& def);
    Definition& Define(Stat::Id stat, Stat::Id dependency=Stat::Id::none, float multiplier=1.0);
    Definition& Define(Stat::Id stat, Stat::Id dependency, Stat::Operator op, Stat::Id operand);
    Definition& Define(Stat::Id stat, int value);
    Definition& Count();
private:
    void Ensure(Stat::Id id);
    StatDefinition& definition;
    Stat::Id last;
};


class CreatureDefinition : public Definition
{
public:
    CreatureDefinition();
};

}
