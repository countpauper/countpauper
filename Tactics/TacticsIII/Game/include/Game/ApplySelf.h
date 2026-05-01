#pragma once
#include "Game/Action.h"

namespace Game
{
class World;

class ApplySelf : public Action
{
public:
    ApplySelf(World& world, Actor& actor, const Condition& condition, unsigned level = 1);
    Requirements CanDo() const override;
    std::vector<Delta> Execute(std::ostream& log) const override;
    std::string Description() const override;
    Position GetDestination() const override;
    unsigned GetDistance() const override;
private:
    std::string TargetConditionName() const;
    const Condition& condition;
    unsigned level;
};

}
