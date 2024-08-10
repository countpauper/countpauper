#include <gtest/gtest.h>
#include "Game/ItemDatabase.h"
#include "Game/Restriction.h"
#include "Definition.h"
#include "Game/Mock/MockBoni.h"
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
    def.Define(Stat::sharp_damage, Stat::offense, 1, {Restriction::sharp});
    def.Define(Stat::blunt_damage, Stat::offense, 1, {Restriction::blunt});
    Item item("weapon", {Restriction::melee, Restriction::blunt});
    item.Set(Stat::offense, 2);
    EXPECT_EQ(item.Get(Stat::sharp_damage).Total(), 0);
    EXPECT_EQ(item.Get(Stat::blunt_damage).Total(), 2);
}

TEST(Item, GetBoni)
{
    Definition def(Item::definition);
    def.Define(Stat::price);

    Item item("item", {});
    item.Set(Stat::price, 3);
    MockBoni boni;
    EXPECT_CALL(boni, Bonus(Stat::price)).WillOnce(Return(Computation(-1)));
    EXPECT_EQ(item.Get(Stat::price, &boni).Total(), 2);
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

TEST(Item, OffenseBonus)
{
    auto json = nlohmann::json::parse(R"""({
        "name":"Weapon",
        "tags":["melee", "blunt"],
        "offense": 3,
        "stat": "damage"
})""");

    Definition def(Item::definition);
    def.Define(Stat::offense);
    Item item(json);
    MockBoni creature;
    EXPECT_CALL(creature, Bonus(Stat::offense)).WillOnce(Return(Computation()));
    EXPECT_CALL(creature, Bonus(Stat::damage)).WillOnce(Return(Computation(2, "damage")));
    EXPECT_EQ(item.Get(Stat::offense, &creature, {Restriction::melee}).Total(), 5);
}

}
