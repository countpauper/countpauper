#pragma once
#include "Game/Boni.h"
#include "Game/Restriction.h"

namespace Game
{

class ItemBonus : public Boni
{
public:
    ItemBonus() = default;
    explicit ItemBonus(const json& data, const Restrictions& extra={});
    virtual ~ItemBonus() = default;
    Computation Bonus(Stat::Id stat) const override;
    bool Match(const Restrictions& filter) const;
private:
    std::string prefix;
    std::string postfix;
    std::map<Stat::Id, int> stats;
    Restrictions restrictions;
};

}
