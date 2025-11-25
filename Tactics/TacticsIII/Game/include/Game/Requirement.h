#pragma once

#include "Game/Stat.h"
#include "Game/Computation.h"
#include "Game/Comparator.h"
#include <vector>
#include <string>
#include <vector>

namespace Game
{

class StatRequirement
{
public:
    StatRequirement(Stat::Id stat, const Computation& actual, Comparator op, const Computation& required);
    explicit operator bool() const;
    bool operator==(const StatRequirement& req) const;
    StatRequirement operator!() const;
    std::string Description() const;
private:
    Stat::Id stat;
    Comparator op;
    Computation actual;
    Computation required;
};

class PathRequirement
{
public:
    explicit PathRequirement(bool reachable);
    explicit operator bool() const;
    std::string Description() const;
    bool operator==(const PathRequirement& req) const;
private:
    bool reachable;
};

// TODO: Item requirement, statE requirement

using RequirementVariant = std::variant<StatRequirement, PathRequirement>;

class Requirement
    : public RequirementVariant
{
public:
    using RequirementVariant::RequirementVariant;

    explicit operator bool() const;
    std::string Description() const;
};

class Requirements : public std::vector<Requirement>
{
public:
    Requirements() = default;
    Requirements(std::initializer_list<Requirement> init);

    operator bool() const;
    Requirements Failed() const;
    Requirements Succeeded() const;

    std::string Description() const;
};

}
