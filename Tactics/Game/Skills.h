#pragma once
#include <memory>
#include <set>
#include <vector>
#include "Item.h"
#include "Trajectory.h"
#include "Targeting.h"
#include "Stats.h"

namespace Engine { class Image; }
namespace Game
{
class Action;
class Actor;
class Target;

class Skill
{
public:
    Skill();
    enum class Trigger { None = 0, Act, Combo, Prepare, React, Defend };
    Skill(const std::wstring name, Trigger trigger, Type::Weapon::Style weapon, Trajectory trajectory, int damage);
    virtual ~Skill();

    Action* CreateAction(const Actor& actor, const Target& target, Trajectory trajectory) const;
    Bonus GetChance(const Score& level) const;

    bool Match(const std::wstring& category) const;
    bool Require(const Weapon* item) const;
    bool Combo(const Skill& previous) const;
    bool Counter(const Skill& previous) const;
    unsigned Id() const;
    Engine::Image Icon() const;
    void LoadIcon(const std::wstring& fname);

    bool IsActive() const;
    bool IsAttack() const;
    bool IsMove() const;
    bool IsWait() const;

    std::wstring name;
    std::wstring description;
    unsigned mp;
    unsigned range;

    
    Trigger trigger;
    using Category = std::wstring;
    using Categories = std::set<Category>;
    Categories categories;
    Categories follows;
    Attribute attribute;
    Attribute resist;
    int offset;
    Type::Weapon::Style weapon;
    using Prerequisite = std::pair<Skill*, unsigned>;
    std::vector<Prerequisite> prerequisites;
    std::vector<int> chance;
    std::set<Trajectory> trajectory;
    std::set<Targeting> target;

    enum class Effect { 
        None,
        Miss,       // no effect on actor
        Interrupt,  // stop action
        Deflect,    // other target within range
        Reflect,    // effect attacker
        Engage,     // control target limb with this skill
        Disarm,     // remove held item
        Steal,      // remove targeted item and give to attacker
        Drop,       // remove targeted item and put it on the floor as an object
        Control,    // control targeted limb
    };
    using Effects = std::set<Effect>;
    Effects effects;

    class Act
    {
    public:
        virtual ~Act() = default;
        virtual Action* CreateAction(const Skill& skill, const Actor& actor, const Target& target, Trajectory trajectory) const = 0;

    };
    class Move : public Act
    {
    public:
        Action* CreateAction(const Skill& skill, const Actor& actor, const Target& target, Trajectory trajectory) const override;
    };
    class Melee : public Act
    {
    public:
        Melee();
        std::vector<unsigned> damage;
        Attribute attribute;
        Bonus DamageBonus(const Score& skillScore);
        Action* CreateAction(const Skill& skill, const Actor& actor, const Target& target, Trajectory trajectory) const override;
    };
    class Affect : public Act
    {
    public:
        Action* CreateAction(const Skill& skill, const Actor& actor, const Target& target, Trajectory trajectory) const override;
    };

    std::shared_ptr<Act> type; // todo: unique ptr and copy/clone or move
private:
    template<class T>
    bool IsType() const 
    { 
        return dynamic_cast<T*>(type.get()) != nullptr; 
    }
    bool Follows(const Skill& previous) const;
    std::shared_ptr<Engine::Image> icon;
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