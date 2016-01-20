#pragma once
#include <memory>
#include <vector>
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
        unsigned cost;
        float range;
    };
    std::wistream& operator>>(std::wistream& s, Skill& skill);

    class Skills : public std::vector < Skill >
    {
    public:
        Skills()=default;
        const Skill* Find(const std::wstring& name) const;
    private:
        friend std::wistream& operator>>(std::wistream& s, Skills& skills);
    };
    std::wistream& operator>>(std::wistream& s, Skills& skills);


}