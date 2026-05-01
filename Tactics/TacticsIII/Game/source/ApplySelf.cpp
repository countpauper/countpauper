#include "Game/UpDown.h"
#include "Game/Condition.h"
#include "Game/Requirement.h"

namespace Game
{


ApplySelf::ApplySelf(World& world, Actor& actor, const Condition& condition, unsigned level) :
    Action(world, actor),
    condition(condition),
    level(level)
{
    this->level = level;
}

Requirements ApplySelf::CanDo() const
{
    return Requirements {
        StatRequirement(Stat::ap, actor.GetCounts().Available(Stat::ap), Comparator::greater_equal, AP()),
        ConditionRequirement(condition, actor.GetConditions().GetCondition(condition), !bool(level))
    };
}

std::vector<Delta> ApplySelf::Execute(std::ostream& log) const
{
    auto can = CanDo();
    if (!can)
    {
        log << actor.GetAppearance().Name() << " can't go " << TargetConditionName() << ", because " << can.Description();
        return {};
    }
    Delta dActor(actor);
    dActor.GetConditions().SetCondition(condition, level);
    dActor.GetCounts().Cost(Stat::ap, AP(), true);
    log << actor.GetAppearance().Name() << " goes " << TargetConditionName();
    return { std::move(dActor) };
}

std::string ApplySelf::Description() const
{
    std::stringstream ss;
    ss << "Go " << TargetConditionName();
    return ss.str();
}

std::string ApplySelf::TargetConditionName() const
{
    if (level == 0)
        return condition.OppositeName();
    else if (level == 1)
        return std::string(condition.Name());
    else
        return std::format("{} ({})", condition.Name(), level);
}

Position ApplySelf::GetDestination() const
{
    return actor.GetPosition();
}

unsigned ApplySelf::GetDistance() const
{
    return 0;
}

}
