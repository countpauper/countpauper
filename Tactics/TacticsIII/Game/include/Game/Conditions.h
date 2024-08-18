#pragma once
#include <vector>
#include "Game/Condition.h"
#include "Game/Boni.h"
#include "File/Json.h"

namespace Game
{

class Conditions : public virtual Boni
{
public:
    Conditions() = default;
    explicit Conditions(const json& data);
    virtual ~Conditions() = default;

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
    Computation Bonus(Stat::Id id) const override;
    std::string Description() const;
    json Serialize() const;
private:
    std::vector<std::unique_ptr<Condition>> conditions;
};

}
