#include <gtest/gtest.h>
#include "Game/ItemDatabase.h"
#include "Game/Restriction.h"
#include "Definition.h"
#include "Game/Mock/MockBoni.h"
#include <nlohmann/json.hpp>

namespace Game::Test
{
using namespace ::testing;

TEST(Item, load_name_and_stat)
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

TEST(Item, damage_type_is_offense)
{
    Definition def(Item::definition);
    def.Define(Stat::sharp_damage, Stat::offense, 1, {Restriction::sharp});
    def.Define(Stat::blunt_damage, Stat::offense, 1, {Restriction::blunt});
    Item item("weapon", {Restriction::melee, Restriction::blunt});
    item.Set(Stat::offense, 2);
    EXPECT_EQ(item.Get(Stat::sharp_damage).Total(), 0);
    EXPECT_EQ(item.Get(Stat::blunt_damage).Total(), 2);
}

TEST(Item, get_extra_bonus)
{
    Definition def(Item::definition);
    def.Define(Stat::price);

    Item item("item", {});
    item.Set(Stat::price, 3);
    MockBoni boni;
    EXPECT_CALL(boni, Bonus(Stat::price)).WillOnce(Return(Computation(-1)));
    EXPECT_EQ(item.Get(Stat::price, &boni).Total(), 2);
}

TEST(Item, load_restrictions)
{

    Definition def(Item::definition);
    def.Define(Stat::offense);
    Item item(json::parse(R"""({
        "name":"Restricted",
        "tags":["melee", "blunt"],
        "offense": 3
})"""));
    EXPECT_TRUE(item.Match({Restriction::melee}));
    EXPECT_TRUE(item.Match({Restriction::blunt}));
    EXPECT_FALSE(item.Match({Restriction::ranged}));
}

TEST(Item, offense_bonus_is_taken_from_creature)
{


    Definition def(Item::definition);
    def.Define(Stat::offense);
    Item item(json::parse(R"""({
        "name":"Weapon",
        "tags":["melee", "blunt"],
        "offense": 3,
        "stat": "damage"
})"""));
    MockBoni creature;
    EXPECT_CALL(creature, Bonus(Stat::offense)).WillOnce(Return(Computation()));
    EXPECT_CALL(creature, Bonus(Stat::damage)).WillOnce(Return(Computation(2, "damage")));
    EXPECT_EQ(item.Get(Stat::offense, &creature, {Restriction::melee}).Total(), 5);
}

}
