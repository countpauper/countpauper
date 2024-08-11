#include <gtest/gtest.h>
#include "Game/Race.h"
#include <nlohmann/json.hpp>

namespace Game::Test
{
using namespace ::testing;

TEST(Race, Bonus)
{
    Race r("orc", {{Stat::str, 2}});
    EXPECT_EQ(r.Bonus(Stat::str), Computation(2, "orc"));
}

TEST(Race, LoadNameAndStat)
{
    auto json = nlohmann::json::parse(R"""({
        "human": {
            "stats": {
                "strength": 1,
                "intelligence": 1,
                "agility": 1,
                "hands": 2
            }
        }
})""");

    auto races = Race::Parse(json);
    ASSERT_EQ(races.size(), 1);
    const Race& human = races.front();
    EXPECT_EQ(human.Name(), "human");
    EXPECT_EQ(human.Bonus(Stat::str), Computation(1, "human"));
}

}
