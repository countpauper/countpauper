#pragma once
#include "Game/Statted.h"
#include <initializer_list>
#include <nlohmann/json.hpp>

namespace Game
{

class Statistics : public Statted
{
public:
    Statistics() = default;
    Statistics(std::initializer_list<std::pair<const Stat::Id, int>> stats);
    virtual ~Statistics() = default;
    Computation Get(Stat::Id id, const class Boni* boni = nullptr, const Restrictions& restricted={}) const override;
    void Level(Stat::Id stat, int amount) override;
    void Set(Stat::Id id, int value);
    void Deserialize(const json& data);
    json Serialize() const;
protected:
    std::map<Stat::Id, int> stats;
};

}
