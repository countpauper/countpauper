#include <gtest/gtest.h>
#include "Game/Restriction.h"
#include <nlohmann/json.hpp>

namespace Game::Test
{
using namespace ::testing;

TEST(Restriction, parse)
{
    auto json = nlohmann::json::parse(R"""({
        "restrict":["melee", "blunt"]
})""");

    auto restriction = Restrictions::Parse(json, "restrict");
    EXPECT_EQ(restriction, Restrictions({Restriction::melee, Restriction::blunt}));
    auto noRestriction = Restrictions::Parse(json, "none");
    EXPECT_EQ(noRestriction, Restrictions());
}

TEST(Restriction, match_conjunction)
{
    EXPECT_TRUE(Restrictions().Match({Restriction::melee}));
    EXPECT_FALSE(Restrictions({Restriction::none}).Match({Restriction::melee}));
    EXPECT_TRUE(Restrictions({Restriction::melee}).Match({Restriction::melee}));
    EXPECT_FALSE(Restrictions({Restriction::ranged}).Match({Restriction::melee}));
    EXPECT_TRUE(Restrictions({Restriction::melee}).Match({Restriction::melee, Restriction::thrown}));
    EXPECT_FALSE(Restrictions({Restriction::item, Restriction::metal}).Match({Restriction::item, Restriction::wood}));
    EXPECT_TRUE(Restrictions({Restriction::item, Restriction::metal, Restriction::leather}).Match({Restriction::item, Restriction::leather}));
}

TEST(Restriction, match_disjunction_in_cattegory)
{
    // the restrictions consist of a conjunction over all mentioned categories but a disjunction of the options in each
    EXPECT_TRUE(Restrictions({Restriction::leather, Restriction::cloth}).Match({Restriction::armor, Restriction::leather}));

}

TEST(Restriction, or_operator_is_inclusive_per_category)
{
    EXPECT_EQ(Restrictions({Restriction::leather}) | Restriction::cloth, Restrictions({Restriction::leather, Restriction::cloth}));
}

TEST(Restriction, xor_operator_is_exclusive_per_category)
{
    EXPECT_EQ(Restrictions({Restriction::melee}) ^ Restriction::ranged, Restrictions({Restriction::ranged}));
}

TEST(Restriction, and_operator_is_intersection)
{
    EXPECT_EQ(Restrictions({Restriction::melee, Restriction::cloth}) & Restrictions({Restriction::ranged, Restriction::melee}), Restrictions({Restriction::melee}));
}

TEST(Restriction, is_empty)
{
    EXPECT_TRUE(Restrictions().empty());
    EXPECT_FALSE(Restrictions({Restriction::air}).empty());
}

TEST(Restriction, access_by_index)
{
    EXPECT_EQ(Restrictions({Restriction::air, Restriction::fire, Restriction::water}).at(1), Restriction::fire);
    EXPECT_EQ(Restrictions({Restriction::air}).at(1), Restriction::none);
}


}
