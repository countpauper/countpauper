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

    std::wstring ToString(Attribute attribute);

    class Attributes : public std::set<Attribute>
    {
    public:
        Attributes() = default;
        Attributes(std::initializer_list<Attribute> init ) : 
            std::set<Attribute>(init)
        {
        }
        static const Attributes All;
        static const std::map<std::wstring, Attribute> map;
    };
    std::wistream& operator>>(std::wistream& s, Attributes& attribute);

    class Stats
    {
    public:
        using Score = int;
        Stats();
        Stats(Score strength, Score agility, Score constitution, Score intelligence, Score wisdom);
        Stats(Attribute attribute, Score score);
        Stats(Attributes attributes, Score allTheSameScore);

        Score strength;
        Score agility;
        Score constitution;
        Score intelligence;
        Score wisdom;
        Score operator[](Attribute attribute) const;
        Score& operator[](Attribute attribute);

        std::wstring Description() const;
    };
    std::wistream& operator>>(std::wistream& s, Stats& actor);

};
