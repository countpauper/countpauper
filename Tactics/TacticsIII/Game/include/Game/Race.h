#pragma once
#include "Game/Boni.h"
#include <map>
#include <string>
#include <initializer_list>

namespace Game
{
class Race : public Boni
{
public:
        Race(std::string_view name, std::initializer_list<std::pair<const Stat::Id, int>> boni = {});
        virtual ~Race() = default;
        std::string_view Name() const;
        Computation Bonus(Stat::Id id) const override;
        const class Item& GetUnarmed() const;
private:
        std::string name;
        std::map<Stat::Id, int> boni;
};

}
