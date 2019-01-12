#pragma once
#include <string>
#include <map>
#include <set>
namespace Game
{
    enum class Attribute
    {
        None = 0,
        Strength = 1,
        Agility = 2,
        Constitution = 3,
        Intelligence = 4,
        Wisdom = 5,
    }; 
    std::wistream& operator>>(std::wistream& s, Attribute& attribute);

    class Attributes : public std::set<Attribute>
    {
    public:
        Attributes() = default;
        Attributes(std::initializer_list<Attribute> init ) : 
            std::set<Attribute>(init)
        {
        }
        static const Attributes All;
    };
    std::wistream& operator>>(std::wistream& s, Attributes& attribute);

    class Stats
    {
    public:
        using Score = int;
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
