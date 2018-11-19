#pragma once
#include <memory>
#include <set>
#include <vector>
#include "Direction.h"
#include "Item.h"

namespace Game
{
class Action;
class Actor;

class Skill
{
public:
    Skill();
    Action* Action(Actor& target) const;

    std::wstring name;
    unsigned mp;
    float range;

    bool IsActive() const;
    enum class Trigger { None=0, Act, Combo, Prepare, React, Defend};
    Trigger trigger;
    using Category = std::string;
    using Categories = std::set<Category>;
    Categories categories;
    Categories follows;
    Type::Weapon::Style weapon;
    using Prerequisite = std::pair<Skill*, unsigned>;
    std::vector<Prerequisite> prerequisites;

    enum class Effect { Miss, Interrupt, Disarm, Stuck, Stop };
    using Effects = std::set<Effect>;
    static const std::map<std::wstring, Effect> effectMap;

    class Type
    {
    public:
        virtual ~Type() = default;
    };
    class Move : public Type
    {
    public:
    };
    class Melee : public Type
    {
    public:
        std::vector<unsigned> damage;
        Effects effects;
    };
    class Affect : public Type
    {
    public:
        std::vector<float> chance;
        Effects effects;
    };

    std::shared_ptr<Type> type; // todo: unique ptr and copy/clone or move
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