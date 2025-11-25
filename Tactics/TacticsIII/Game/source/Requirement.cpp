#include "Game/Requirement.h"
#include "Game/Computation.h"
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


bool StatRequirement::operator==(const StatRequirement& o) const
{
    return stat == o.stat &&
        actual == o.actual &&
        op == o.op &&
        required == o.required;
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
