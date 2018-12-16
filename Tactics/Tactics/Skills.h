#pragma once
#include <memory>
#include <set>
#include <vector>
#include "Item.h"

namespace Game
{
class Action;
class Actor;
class Target;

class Skill
{
public:
    Skill();
    Action* CreateAction(const Actor& actor, const Target& target) const;
    Bonus GetChance(const Score& level) const;

    bool Follows(const Skill& previous) const;

    std::wstring name;
    unsigned mp;
    float range;

    bool IsActive() const;
    bool IsAttack() const;
    bool IsMove() const;
    enum class Trigger { None = 0, Act, Combo, Prepare, React, Defend };
    Trigger trigger;
    using Category = std::string;
    using Categories = std::set<Category>;
    Categories categories;
    Categories follows;
    Type::Weapon::Style weapon;
    using Prerequisite = std::pair<Skill*, unsigned>;
    std::vector<Prerequisite> prerequisites;
    std::vector<int> chance;

    enum class Effect { Miss, Interrupt, Disarm, Stuck, Stop };
    using Effects = std::set<Effect>;
    static const std::map<std::wstring, Effect> effectMap;

    class Type
    {
    public:
        virtual ~Type() = default;
        virtual Action* CreateAction(const Skill& skill, const Actor& actor, const Target& target) const = 0;

    };
    class Move : public Type
    {
    public:
        Action* CreateAction(const Skill& skill, const Actor& actor, const Target& target) const override;
    };
    class Melee : public Type
    {
    public:
        std::vector<unsigned> damage;
        Effects effects;
        Bonus DamageBonus(const Score& skillScore);
        Action* CreateAction(const Skill& skill, const Actor& actor, const Target& target) const override;
    };
    class Affect : public Type
    {
    public:
        Effects effects;
        Action* CreateAction(const Skill& skill, const Actor& actor, const Target& target) const override;
    };

    std::shared_ptr<Type> type; // todo: unique ptr and copy/clone or move
private:
    template<class T>
    bool IsType() const 
    { 
        return dynamic_cast<T*>(type.get()) != nullptr; 
    }
};


class Skills : public std::vector<Skill>
{
public:
    Skills() = default;
    Skills(const std::string& filename);
    const Skill* Find(const std::wstring& name) const;
private:
    Skills(const std::vector<Skill>& skills);
    void Load(const std::string& fn);
};

}