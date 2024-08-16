#include "Game/StatDefinition.h"
#include "Game/Stat.h"

namespace Game::Test
{

class Definition
{
public:
    explicit Definition(StatDefinition& def);
    Definition& Define(Stat::Id stat, Stat::Id dependency=Stat::Id::none, float multiplier=1.0, const Restrictions& restrict={});
    Definition& Define(Stat::Id stat, Stat::Id dependency, Operator op, Stat::Id operand , const Restrictions& restrict={});
    Definition& Define(Stat::Id stat, int value, const Restrictions& restrict={});
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
