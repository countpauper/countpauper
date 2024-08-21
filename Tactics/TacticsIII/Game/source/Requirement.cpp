#include "Game/Requirement.h"
#include "Game/Computation.h"
#include "Utility/String.h"

namespace Game
{

Requirement::Requirement(Stat::Id stat, const Computation& actual, Operator op, const Computation& required) :
    stat(stat),
    actual(actual),
    op(op),
    required(required)
{
}

Requirement::operator bool() const
{
    switch(op)
    {
        case not_equal:
            return actual.Total() != required.Total();
        case equal:
            return actual.Total() == required.Total();
        case less:
            return actual.Total() < required.Total();
        case less_equal:
            return actual.Total() <= required.Total();
        case greater:
            return actual.Total() > required.Total();
        case greater_equal:
            return actual.Total() >= required.Total();
        default:
            assert(false); // unimplemented
            return false;
    }
}

bool Requirement::operator==(const Requirement& o) const
{
    return stat == o.stat &&
        actual == o.actual &&
        op == o.op &&
        required == o.required;
}

Requirement Requirement::operator!() const
{
    switch(op)
    {
        case not_equal:
            return Requirement(stat, actual, equal, required);
        case equal:
            return Requirement(stat, actual, not_equal, required);
        case less:
            return Requirement(stat, actual, greater_equal, required);
        case less_equal:
            return Requirement(stat, actual, greater, required);
        case greater:
            return Requirement(stat, actual, less_equal, required);
        case greater_equal:
            return Requirement(stat, actual, less, required);
        default:
            assert(false); // unimplemented
            return *this;
    }
}

std::string_view Requirement::Describe(Operator op)
{
    switch(op)
    {
        case not_equal:
            return "is not";
        case equal:
            return "is";
        case less:
            return "is less than";
        case less_equal:
            return "is not more than";
        case greater:
            return "is more than";
        case greater_equal:
            return "is not less than";
        default:
            assert(false); // unimplemented
            return "";
    }
}

std::string Requirement::Description() const
{
    if (bool(*this))
    {
        return std::string(Stat::Name(stat)) + " (" + actual.Description() +") " + std::string(Describe(op)) + " " + required.Description();
    }
    else
    {
        return (!*this).Description();
    }
}


Requirements::Requirements(std::initializer_list<Requirement> init) :
    std::vector<Requirement>(init)
{
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
