#pragma once
#include <string>
#include <map>

namespace Game
{
    enum class Attribute
    {
        None = 0,
        Strength = 1<<0,
        Agility = 1<<1,
        Constitution = 1<<2,
        Intelligence = 1<<3,
        Wisdom = 1<<4,
        All = Strength + Agility + Constitution + Intelligence + Wisdom
    }; 
    std::wistream& operator>>(std::wistream& s, Attribute& attribute);

    class Stats
    {
    public:
        using Score = unsigned;
        Stats();
        Stats(Score strength, Score agility, Score constitution, Score intelligence, Score wisdom) :
            strength(strength),
            agility(agility),
            constitution(constitution),
            intelligence(intelligence),
            wisdom(wisdom)
        {
        }
        Score strength;
        Score agility;
        Score constitution;
        Score intelligence;
        Score wisdom;
        std::wstring Description() const;
    };
    std::wistream& operator>>(std::wistream& s, Stats& actor);

};
