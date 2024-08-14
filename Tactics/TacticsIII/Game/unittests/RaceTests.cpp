#include <gtest/gtest.h>
#include "Game/Race.h"
#include <nlohmann/json.hpp>

namespace Game::Test
{
using namespace ::testing;

TEST(Race, bonus)
{
    Race r("orc", {{Stat::str, 2}});
    EXPECT_EQ(r.Bonus(Stat::str), Computation(2, "orc"));
}

TEST(Race, load_name_and_stats_from_json)
{
    Races races(json::parse(R"""({
        "human": {
            "stats": {
                "strength": 1,
                "intelligence": 1,
                "agility": 1,
                "hands": 2
            }
        }
})"""));
    ASSERT_EQ(races.size(), 1);
    const Race& human = races.front();
    EXPECT_EQ(human.Name(), "human");
    EXPECT_EQ(human.Bonus(Stat::str), Computation(1, "human"));
    EXPECT_EQ(races.Find("human")->Name(), "human");
    EXPECT_EQ(races.Find("marathon"), nullptr);
}

}
