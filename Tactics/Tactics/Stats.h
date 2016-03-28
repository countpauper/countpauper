#pragma once
#include <string>
#include <map>

namespace Game
{
    class Stats
    {
    public:
        using Stat = int;
        Stats();
        Stat strength;
        Stat agility;
        Stat constitution;
        Stat intelligence;
        std::wstring Description() const;
    };
    std::wistream& operator>>(std::wistream& s, Stats& actor);

};