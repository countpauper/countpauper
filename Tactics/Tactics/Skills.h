#pragma once
#include <memory>
#include <vector>
namespace Game
{
    class Action;

    class Skill
    {
    public:
        Skill();
        std::unique_ptr<Action> Action() const;
        std::wstring name;

    private:
        friend std::wistream& operator>>(std::wistream& s, Skill& skill);
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