#pragma once
#include "Game/Boni.h"
#include <map>
#include <string>
#include <initializer_list>
#include <nlohmann/json.hpp>
#include "Geometry/Size.h"

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
    Engine::Size GetSize() const;
private:
    std::string name;
    Engine::Size size;
    std::map<Stat::Id, int> boni;
};

class Races : public std::vector<Race>
{
public:
    Races() = default;
    explicit Races(const json& data);
    const Race* Find(std::string_view name) const;
    const Race& Get(std::string_view name) const;
};


}
