#pragma once
#include <vector>
#include "Game/Condition.h"
#include "Game/Boni.h"
#include "File/Json.h"
#include "Utility/Singleton.h"

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
        return std::find_if(conditions.begin(), conditions.end(), [](const decltype(conditions)::value_type& c)
        {
            return dynamic_cast<const CT*>(c.first) != nullptr;
        }) != conditions.end();
    }

    void Apply(const Condition& condition, unsigned level = 1)
    {
        auto [it, inserted] = conditions.insert(std::make_pair(&condition,level));
        if (!inserted)
        {
            it->second = std::max(level, it->second);
        }
    }
    template<class CT>
    void Apply(unsigned level = 1)
    {
        Apply(*Engine::Singleton<CT>(), level);
    }

    Computation Bonus(Stat::Id id) const override;
    std::string Description() const;
    json Serialize() const;
private:
    std::map<const Condition*, unsigned> conditions;
};

}
