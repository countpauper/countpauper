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


ConditionRequirement::ConditionRequirement(const Condition& condition, unsigned actual, Comparator op, unsigned required) :
    condition(condition),
    actual(actual),
    required(required),
    op(op)
{
}


ConditionRequirement::ConditionRequirement(const Condition& condition, unsigned has, bool expected) :
    ConditionRequirement(condition, has, expected ? Comparator::not_equal : Comparator::equal, 0)
{
}

ConditionRequirement::operator bool() const
{
    return op(actual, required);
}

bool ConditionRequirement::operator==(const ConditionRequirement& rhs) const
{
    return actual == rhs.actual && 
        required == rhs.required && 
        &condition == &rhs.condition && 
        op == rhs.op;
}



ConditionRequirement ConditionRequirement::operator!() const
{
    return ConditionRequirement(condition, actual, op.Opposite(), required);
}

std::string ConditionRequirement::Description() const
{
    if (IsBooleanRequirement())
    {
          if (bool(*this))
          {
                if (actual)
                    return "is " + std::string(condition.Name());
                else
                    return "is " + condition.OppositeName();
            }
            else
            {
                if (actual)
                    return "isn't " + condition.OppositeName();
                else
                    return "isn't " + std::string(condition.Name());
            }
    }
    else 
    {
          if (bool(*this))
          {
            return std::format("{}({}) {} {}", std::string(condition.Name()), actual, std::string(op), required);
        }
        else
        {
            return (!*this).Description();
        }
    }
}

bool ConditionRequirement::IsBooleanRequirement() const
{
    return required==0 && 
            (op==Comparator::equal || op==Comparator::not_equal);
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
        return Engine::Join(failStrings, " and ");
    }
}


}
