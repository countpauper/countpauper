#pragma once
#include <vector>
#include "Game/Condition.h"

namespace Game
{

class Conditions
{
public:
    Conditions() = default;

    template<class CT>
    bool Is() const
    {
        return std::find_if(conditions.begin(), conditions.end(), [](const std::unique_ptr<Condition>& c)
        {
            return dynamic_cast<const CT*>(c.get()) != nullptr;
        }) != conditions.end();
    }

    template<class CT>
    void Apply()
    {
        // TODO: power level that replaces lower
        if (!Is<CT>())
        {
            conditions.emplace_back(std::make_unique<CT>());
        }
    }
    Computation Boni(Stat::Id id) const;
    std::string Description() const;
private:
    std::vector<std::unique_ptr<Condition>> conditions;
};

}
