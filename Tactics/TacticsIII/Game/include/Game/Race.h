#pragma once
#include "Game/Boni.h"
#include <map>
#include <string>
#include <initializer_list>
#include <nlohmann/json.hpp>

namespace Game
{
class Race : public Boni
{
public:
    Race(std::string_view name, std::initializer_list<std::pair<const Stat::Id, int>> boni = {});
    explicit Race(std::string_view name, const json& raceData);
    virtual ~Race() = default;
    std::string_view Name() const;
    Computation Bonus(Stat::Id id) const override;
    const class Item& GetUnarmed() const;

    static std::vector<Race> Parse(const json& data);
private:
    std::string name;
    std::map<Stat::Id, int> boni;
};

}
