#pragma once
#include "Game/Boni.h"

namespace Game
{

class ItemBonus : public Boni
{
public:
    ItemBonus() = default;
    explicit ItemBonus(const json& data);
    virtual ~ItemBonus() = default;
    Computation Bonus(Stat::Id stat) const override;
private:
    std::string prefix;
    std::string postfix;
    std::map<Stat::Id, int> stats;
};

}
