#include "Game/Requirement.h"
#include "Game/Computation.h"

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
            return "not equal to";
        case equal:
            return "equal to";
        case less:
            return "less than";
        case less_equal:
            return "not more than";
        case greater:
            return "more than";
        case greater_equal:
            return "not less than";
        default:
            assert(false); // unimplemented
            return "";
    }
}

std::string Requirement::Description() const
{
    if (bool(*this))
    {
        return std::string(Stat::Name(stat)) + "(" + actual.Description() +") is " + std::string(Describe(op)) + " " + required.Description();
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



}
