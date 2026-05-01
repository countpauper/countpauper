#pragma once
#include <vector>
#include "Game/Condition.h"
#include "Game/Boni.h"
#include "File/Json.h"
#include "Utility/Singleton.h"
#include <ranges>

namespace Game
{


class Conditions : public virtual Boni
{
public:
    virtual unsigned GetCondition(std::function<bool(const Condition& condition)> pred) const = 0;
    unsigned GetCondition(const Condition& condition) const;
    bool Has(std::function<bool(const Condition& condition)> pred) const;
    virtual void SetCondition(const Condition& condition, unsigned level =1) = 0;

    template<class CT>
    bool Is() const
    {
        return Has([](const Condition& condition)
        {
            return dynamic_cast<const CT*>(&condition) != nullptr;
        });
    }
    
    template<class CT>
    void Apply(unsigned level = 1)
    {
        SetCondition(*Engine::Singleton<CT>(), level);
    }
    virtual Computation ConditionalBonus(Stat::Id id) const  = 0;
    Computation Bonus(Stat::Id id) const override;
    using Range= std::ranges::subrange<std::map<const Condition*, unsigned>::const_iterator>;
    virtual void ApplyConditions(Range conditions) = 0;
};

class ConditionLevels : public Conditions
{
public:
    ConditionLevels() = default;
    explicit ConditionLevels(const json& data);
    ConditionLevels(const ConditionLevels& other);
    ConditionLevels(ConditionLevels&& other);
    virtual ~ConditionLevels() = default;

    std::pair<const Condition*, unsigned> FindCondition(const std::function<bool(const Condition& condition)>& pred) const;
    unsigned GetCondition(std::function<bool(const Condition& condition)> pred) const;
    void SetCondition(const Condition& condition, unsigned level = 1);

    Range GetConditions() const;
    void ApplyConditions(Range conditions) override;

    Computation ConditionalBonus(Stat::Id id) const override;
    std::string Description() const;
    json Serialize() const;
private:
    std::map<const Condition*, unsigned> conditions;
};

}
