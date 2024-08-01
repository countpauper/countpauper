#pragma once
#include "Game/Statted.h"
#include <initializer_list>

namespace Game
{

class Statistics : public Statted
{
public:
    Statistics() = default;
    Statistics(std::initializer_list<std::pair<const Stat::Id, int>> stats);
    StatDescriptor Get(Stat::Id id) const override;
    void Level(Stat::Id stat, int amount) override;
protected:
    std::map<Stat::Id, int> stats;
};

}
