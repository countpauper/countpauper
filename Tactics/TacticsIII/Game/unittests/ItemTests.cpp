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
        "damage": 3
})""");

    Definition def(Item::definition);
    def.Define(Stat::damage);
    Item item(json);
    EXPECT_EQ(item.Name(), "Test");
    EXPECT_EQ(item.Get(Stat::damage).Total(), 3);
}

TEST(Item, RetrictionConjunction)
{
    EXPECT_TRUE(Match(Restrictions{Restriction::melee}, Restrictions{}));
    EXPECT_TRUE(Match(Restrictions{Restriction::melee}, Restrictions{Restriction::melee}));
    EXPECT_FALSE(Match(Restrictions{Restriction::melee}, Restrictions{Restriction::ranged}));
    EXPECT_TRUE(Match(Restrictions{Restriction::melee, Restriction::thrown}, Restrictions{Restriction::melee}));
}

TEST(Item, RetrictionDisjunction)
{
    // the restrictions consist of a conjunction over all mentioned categories but a disjunction of the options in each
    EXPECT_TRUE(Match(Restrictions{Restriction::armor, Restriction::leather}, Restrictions{Restriction::leather, Restriction::cloth}));

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
