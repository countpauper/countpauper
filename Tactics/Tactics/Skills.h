#pragma once
#include <memory>
#include <set>
#include <vector>
#include "Direction.h"

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
    enum class Trigger { Act, React, Defend };
    Trigger trigger;
    using Label = std::string;
    using Labels = std::set<Label>;
    Labels label;
    Labels follows;
    using Prerequisite = std::pair<Skill*, unsigned>;
    std::vector<Prerequisite> prerequisites;

    enum class Effect { Miss, Interrupt, Disarm, Stuck, Stop };
    using Effects = std::set<Effect>;

    class Category
    {
    public:
        virtual ~Category() = default;
    };
    class Move : public Category
    {
    public:

    };
    class Melee : public Category
    {
    public:
        std::vector<unsigned> damage;
        Effects effects;
    };

    std::shared_ptr<Category> category; // todo: unique ptr and copy/clone or move
};


class Skills : public std::vector<Skill>
{
public:
    Skills() = default;
    const Skill* Find(const std::wstring& name) const;
    static Skills Load(std::wistream& file);
private:
    Skills(const std::vector<Skill>& skills);
};

}