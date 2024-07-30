#include <gtest/gtest.h>
#include "Game/Counter.h"
#include "Game/Stat.h"
#include "Game/Mock/MockStatted.h"

namespace Game::Test
{
using namespace ::testing;

TEST(Counter, identity)
{
    Counter counter("test", Stat::Id::hp);
    EXPECT_EQ(counter.Name(), std::string_view("test"));
    EXPECT_TRUE(counter.ForStat(Stat::Id::hp));
    EXPECT_FALSE(counter.ForStat(Stat::Id::ap));
}

TEST(Counter, define_with_json)
{
    Counter counter(Stat::Id::hp, json::parse(
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
    Counter counter("test", Stat::Id::hp, Counter::Reset::round);
    EXPECT_TRUE(counter.ResetWhen(Counter::Reset::battle));
    EXPECT_FALSE(counter.ResetWhen(Counter::Reset::turn));
    EXPECT_FALSE(counter.ResetWhen(Counter::Reset::action));
}

TEST(Counter, available)
{
    MockStatted stats;
    EXPECT_CALL(stats, Get(Stat::hp)).WillRepeatedly(Return(StatDescriptor(5)));
    Counter counter("test", Stat::Id::hp);
    EXPECT_EQ(counter.Available(0, stats), 5);
    EXPECT_EQ(counter.Available(2, stats), 3);
    EXPECT_EQ(counter.Available(6, stats), 0);
}

TEST(Counter, available_follows_max)
{
    MockStatted stats;
    EXPECT_CALL(stats, Get(Stat::ap)).WillOnce(Return(StatDescriptor(3)));
    Counter counter("actions", Stat::Id::ap);
    EXPECT_EQ(counter.Available(0, stats), 3);
    EXPECT_CALL(stats, Get(Stat::ap)).WillOnce(Return(StatDescriptor(1)));
    EXPECT_EQ(counter.Available(0, stats), 1);

    EXPECT_CALL(stats, Get(Stat::ap)).WillOnce(Return(StatDescriptor(1)));
    EXPECT_EQ(counter.Available(2, stats), 0);
}
}
