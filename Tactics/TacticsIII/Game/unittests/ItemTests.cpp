#include <gtest/gtest.h>
#include "Game/ItemDatabase.h"
#include "Game/Restriction.h"
#include "Definition.h"
#include <nlohmann/json.hpp>

namespace Game::Test
{
using namespace ::testing;

TEST(Item, LoadNameAndStat)
{
    auto json = nlohmann::json::parse(R"""({
        "name":"Test",
        "offense": 3
})""");

    Definition def(Item::definition);
    def.Define(Stat::offense);
    Item item(json);
    EXPECT_EQ(item.Name(), "Test");
    EXPECT_EQ(item.Get(Stat::offense).Total(), 3);
}

TEST(Item, RetrictionConjunction)
{
    EXPECT_TRUE(Restrictions().Match({Restriction::melee}));
    EXPECT_FALSE(Restrictions({Restriction::none}).Match({Restriction::melee}));
    EXPECT_TRUE(Restrictions({Restriction::melee}).Match({Restriction::melee}));
    EXPECT_FALSE(Restrictions({Restriction::ranged}).Match({Restriction::melee}));
    EXPECT_TRUE(Restrictions({Restriction::melee}).Match({Restriction::melee, Restriction::thrown}));
}

TEST(Item, RetrictionDisjunction)
{
    // the restrictions consist of a conjunction over all mentioned categories but a disjunction of the options in each
    EXPECT_TRUE(Restrictions({Restriction::leather, Restriction::cloth}).Match({Restriction::armor, Restriction::leather}));

}

TEST(Item, DamageType)
{
    Definition def(Item::definition);
    def.Define(Stat::sharp_damage, Stat::damage, 1, {Restriction::sharp});
    def.Define(Stat::blunt_damage, Stat::damage, 1, {Restriction::blunt});
    Item item("weapon", {Restriction::melee, Restriction::blunt});
    item.Set(Stat::damage, 2);
    EXPECT_EQ(item.Get(Stat::sharp_damage).Total(), 0);
    EXPECT_EQ(item.Get(Stat::blunt_damage).Total(), 2);

}
TEST(Item, LoadRetrictions)
{
    auto json = nlohmann::json::parse(R"""({
        "name":"Restricted",
        "tags":["melee", "blunt"],
        "damage": 3
})""");

    Definition def(Item::definition);
    Item item(json);
    EXPECT_TRUE(item.Match({Restriction::melee}));
    EXPECT_TRUE(item.Match({Restriction::blunt}));
    EXPECT_FALSE(item.Match({Restriction::ranged}));
}


}
