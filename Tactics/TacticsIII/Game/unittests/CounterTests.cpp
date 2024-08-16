#include <gtest/gtest.h>
#include "Game/Counter.h"
#include "Game/Counters.h"
#include "Game/Stat.h"
#include "Game/Mock/MockStatted.h"
#include "Definition.h"

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
    EXPECT_CALL(stats, Get(Stat::hp, _, _)).WillRepeatedly(Return(Computation(5)));
    Counter counter("test", Stat::Id::hp);
    EXPECT_EQ(counter.Available(0, stats), 5);
    EXPECT_EQ(counter.Available(2, stats), 3);
    EXPECT_EQ(counter.Available(6, stats), 0);
}

TEST(Counter, available_follows_max)
{
    MockStatted stats;
    EXPECT_CALL(stats, Get(Stat::ap, _, _)).WillOnce(Return(Computation(3)));
    Counter counter("actions", Stat::Id::ap);
    EXPECT_EQ(counter.Available(0, stats), 3);
    EXPECT_CALL(stats, Get(Stat::ap, _, _)).WillOnce(Return(Computation(1)));
    EXPECT_EQ(counter.Available(0, stats), 1);

    EXPECT_CALL(stats, Get(Stat::ap, _, _)).WillOnce(Return(Computation(1)));
    EXPECT_EQ(counter.Available(2, stats), 0);
}

TEST(Counter, Serialize)
{
    MockStatted stats;
    Definition def(MockStatted::definition);
    def.Define(Stat::hp).Count();
    EXPECT_CALL(stats, Get(Stat::hp, nullptr, _)).WillRepeatedly(Return(Computation(3, "mock hp")));
    Counters counters(stats.Definition(), stats);
    counters.Cost(Stat::hp, 1);
    ASSERT_EQ(counters.Available(Stat::hp), 2);
    auto serialized = counters.Serialize();
    Counters deserialized(stats.Definition(), stats, serialized);
    deserialized.Cost(Stat::hp, 1);
    EXPECT_EQ(deserialized.Available(Stat::hp), 1);
}
}
