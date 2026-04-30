#include "Game/Requirement.h"
#include "Game/Computation.h"
#include "Game/Condition.h"
#include "Utility/String.h"

namespace Game
{

StatRequirement::StatRequirement(Stat::Id stat, const Computation& actual, Comparator op, const Computation& required) :
    stat(stat),
    actual(actual),
    op(op),
    required(required)
{
}

StatRequirement::operator bool() const
{
    return op(actual.Total(), required.Total());
}


bool StatRequirement::operator==(const StatRequirement& rhs) const
{
    return stat == rhs.stat &&
        actual == rhs.actual &&
        op == rhs.op &&
        required == rhs.required;
}



StatRequirement StatRequirement::operator!() const
{
    return StatRequirement(stat, actual, op.Opposite(), required);
}

std::string StatRequirement::Description() const
{
    if (bool(*this))
    {
        return std::string(Stat::Name(stat)) + " (" + actual.Description() +") " + std::string(op) + " " + required.Description();
    }
    else
    {
        return (!*this).Description();
    }
}


ConditionRequirement::ConditionRequirement(bool has, const Condition& condition, bool expected) :
    has(has),
    condition(condition),
    expected(expected)
{
}

ConditionRequirement::operator bool() const
{
    return has == expected;
}


bool ConditionRequirement::operator==(const ConditionRequirement& rhs) const
{
    return has == rhs.has && expected == rhs.expected;
}



ConditionRequirement ConditionRequirement::operator!() const
{
    return ConditionRequirement(has, condition, !expected);
}

std::string ConditionRequirement::Description() const
{
    if (bool(*this))
    {
        if (expected)
            return "is " + std::string(condition.Name());
        else
            return "isn't " + std::string(condition.Name());
    }
    else
    {
        if (expected)
            return "isn't " + std::string(condition.Name()) + " but should be";
        else
            return "is " + std::string(condition.Name()) + " but shouldn't be";
    }
}

PathRequirement::PathRequirement(bool reachable)
    : reachable(reachable)
{
}

PathRequirement::operator bool() const
{
    return reachable;
}

std::string PathRequirement::Description() const
{
    if (reachable)
        return "the destination is reachable";
    else
        return "the destination is not reachable";
}

bool PathRequirement::operator==(const PathRequirement& req) const
{
    return reachable == req.reachable;
}

TargetRequirement::TargetRequirement(float surface)
    : surface(surface)
{
}

TargetRequirement::operator bool() const
{
    return surface>=threshold;
}

std::string TargetRequirement::Description() const
{
    if (!bool(*this))
        return "the target is behind cover";
    else if (surface>1.0f)
        return "the target is wide open";
    else 
        return "the target is open";
}

bool TargetRequirement::operator==(const TargetRequirement& req) const
{
    return surface == req.surface;
}

Requirements::Requirements(std::initializer_list<Requirement> init) :
    std::vector<Requirement>(init)
{
}



Requirement::operator bool() const
{
    return std::visit([](const auto& var)
    {
        return bool(var);
    }, *this);
}

std::string Requirement::Description() const
{
    return std::visit([](const auto& var)
    {
        return var.Description();
    }, *this);
}

Requirements::operator bool() const
{
    for(const auto& req: *this)
    {
        if (!req)
            return false;
    }
    return true;
}


Requirements Requirements::Failed() const
{
    Requirements result;
    std::copy_if(begin(), end(), std::back_inserter(result), [](const Requirement& req)
    {
        return !bool(req);
    });
    return result;
}

Requirements Requirements::Succeeded() const
{
    Requirements result;
    std::copy_if(begin(), end(), std::back_inserter(result), [](const Requirement& req)
    {
        return bool(req);
    });
    return result;

}

std::string Requirements::Description() const
{
    auto failed = Failed();
    auto succeeded = Succeeded();
    std::vector<std::string> failStrings, successStrings;
    std::transform(failed.begin(), failed.end(), std::back_inserter(failStrings), [](const Requirement& req)
    {
        return req.Description();
    });
    std::transform(succeeded.begin(), succeeded.end(), std::back_inserter(successStrings), [](const Requirement& req)
    {
        return req.Description();
    });
    if (failed.empty())
    {
        return Engine::Join(successStrings, " and ");
    }
    else if (successStrings.empty())
    {
        return Engine::Join(failStrings, " and ");
    }
    else
    {
        return Engine::Join(successStrings, "and ")+", but "+Engine::Join(failStrings, " and ");
    }
}


}
