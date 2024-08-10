#include <gtest/gtest.h>
#include "Game/Restriction.h"
#include <nlohmann/json.hpp>

namespace Game::Test
{
using namespace ::testing;

TEST(Restriction, Parse)
{
    auto json = nlohmann::json::parse(R"""({
        "restrict":["melee", "blunt"]
})""");

    auto restriction = Restrictions::Parse(json, "restrict");
    EXPECT_EQ(restriction, Restrictions({Restriction::melee, Restriction::blunt}));
    auto noRestriction = Restrictions::Parse(json, "none");
    EXPECT_EQ(noRestriction, Restrictions());
}

TEST(Restriction, MatchConjunction)
{
    EXPECT_TRUE(Restrictions().Match({Restriction::melee}));
    EXPECT_FALSE(Restrictions({Restriction::none}).Match({Restriction::melee}));
    EXPECT_TRUE(Restrictions({Restriction::melee}).Match({Restriction::melee}));
    EXPECT_FALSE(Restrictions({Restriction::ranged}).Match({Restriction::melee}));
    EXPECT_TRUE(Restrictions({Restriction::melee}).Match({Restriction::melee, Restriction::thrown}));
}

TEST(Restriction, MatchDisjunction)
{
    // the restrictions consist of a conjunction over all mentioned categories but a disjunction of the options in each
    EXPECT_TRUE(Restrictions({Restriction::leather, Restriction::cloth}).Match({Restriction::armor, Restriction::leather}));

}

TEST(Restriction, Or)
{
    EXPECT_EQ(Restrictions({Restriction::leather}) | Restriction::cloth, Restrictions({Restriction::leather, Restriction::cloth}));
}

TEST(Restriction, And)
{
    EXPECT_EQ(Restrictions({Restriction::melee}) & Restriction::ranged, Restrictions({Restriction::ranged}));
}

}
