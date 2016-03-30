#pragma once
#include <string>
#include <map>

namespace Game
{
    class Stats
    {
    public:
        using Stat = unsigned;
        Stats();
        Stat strength;
        Stat agility;
        Stat constitution;
        Stat intelligence;
        Stat wisdom;
        std::wstring Description() const;
    };
    std::wistream& operator>>(std::wistream& s, Stats& actor);

};
