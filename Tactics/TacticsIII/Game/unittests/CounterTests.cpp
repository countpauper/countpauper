#include <gtest/gtest.h>
#include "Game/Counter.h"
#include "Game/Stat.h"
#include "Game/Mock/MockStatted.h"

namespace Game::Test
{
using namespace ::testing;

TEST(Counter, identity)
{
    Stat hp("hp");
    Counter counter("test", &hp);
    EXPECT_EQ(counter.Name(), std::string_view("test"));
    EXPECT_TRUE(counter.ForStat(&hp));
    EXPECT_FALSE(counter.ForStat(nullptr));
}

TEST(Counter, define_with_json)
{
    Stat hp("hp");
    Counter counter(hp, json::parse(
    R"""({
                "name": "bar",
                "reset": "turn",
                "reset_to_min": true
        })"""));
    EXPECT_EQ(counter.Name(), std::string_view("bar"));
    EXPECT_TRUE(counter.ResetWhen(Counter::Reset::turn));
}


TEST(Counter, reset_when)
{
    Stat hp("hitpoints");
    Counter counter("test", &hp, Counter::Reset::round);
    EXPECT_TRUE(counter.ResetWhen(Counter::Reset::battle));
    EXPECT_FALSE(counter.ResetWhen(Counter::Reset::turn));
    EXPECT_FALSE(counter.ResetWhen(Counter::Reset::action));
}

TEST(Counter, available)
{
    MockStatted stats;
    Stat hp("hitpoints");
    EXPECT_CALL(stats, Get(Stat::hp)).WillRepeatedly(Return(StatDescriptor(5)));
    Counter counter("test", &hp);
    EXPECT_EQ(counter.Available(0, stats), 5);
    EXPECT_EQ(counter.Available(2, stats), 3);
    EXPECT_EQ(counter.Available(6, stats), 0);
}

}
