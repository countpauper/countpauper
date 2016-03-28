#pragma once
#include <string>
#include <map>

namespace Game
{
    typedef int Pain;
    const Pain MaxPain = 8;
    const Pain ImmunePain = -1;
    enum class Sharp : Pain
    {
        Immune = ImmunePain,
        Healthy = 0,
        Scratched = 1,
        Bleeding = 2,
        Hemorrhaging = 4,
        Severed = MaxPain,
    };

    enum class Crush : Pain
    {
        Immune = ImmunePain,
        Healthy = 0,
        Bruised = 1,
        Battered = 2,
        Broken = 4,
        Crushed = MaxPain
    };

    enum class Fire : Pain
    {
        Immune = ImmunePain,
        Healthy = 0,
        Singed = 1,
        Scorched = 2,
        Burned = 4,
        Charred = MaxPain
    };

    enum class Disease : Pain
    {
        Immune = ImmunePain,
        Healthy = 0,
        Infected = 1,
        Festering = 2,
        Decaying = 4,
        Necrotic = MaxPain,
    };

    struct Wounds
    {
        enum class Type
        {
            Crush = 0,
            Sharp,
            Fire,
            Disease,
            Max
        };
        std::wstring description;
        int penalty;
        typedef std::map<Wounds::Type, std::map<Pain, Wounds>> Table;
        static const Table table;
    };

    class Health
    {
    public:
        Sharp sharp;
        Crush crush;
        Fire burn;
        Disease disease;
        
        void Cut(Sharp damage);
        void Hit(Crush damage);
        void Burn(Fire damage);
        void Inflict(Disease damage);
        std::wstring Description() const;
        bool Dead() const;
    protected:
        std::pair<Wounds::Type, int> FindWorst() const;
    };
    std::wistream& operator>>(std::wistream& s, Health& actor);

};
