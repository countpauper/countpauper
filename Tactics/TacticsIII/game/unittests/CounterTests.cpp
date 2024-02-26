#include <gtest/gtest.h>
#include "Game/Counter.h"
#include "Game/Mock/MockStatted.h"

namespace Game::Test
{
using namespace ::testing;

TEST(Counter, Name)
{
    Counter counter("test", Stat::hp);
    EXPECT_EQ(counter.Name(), std::string_view("test"));
}

TEST(Counter, ResetWhen)
{
    Counter counter("test", Stat::hp, Counter::Reset::round);
    EXPECT_TRUE(counter.ResetWhen(Counter::Reset::battle));
    EXPECT_FALSE(counter.ResetWhen(Counter::Reset::turn));
    EXPECT_FALSE(counter.ResetWhen(Counter::Reset::action));
}

TEST(Counter, Range)
{
    MockStatted stats;
    EXPECT_CALL(stats, Get(Stat::hp)).WillOnce(Return(StatDescriptor(5)));
    Counter counter("test", Stat::hp);
    EXPECT_EQ(counter.GetRange(stats), Engine::Range(0, 5));
}

TEST(Counter, ResetToMax)
{
    MockStatted stats;
    EXPECT_CALL(stats, Get(Stat::ap)).WillOnce(Return(StatDescriptor(3)));
    Counter counter("test", Stat::ap);
    EXPECT_EQ(counter.ResetTo(stats), 3);
}

TEST(Counter, ResetToMin)
{
    MockStatted stats;
    EXPECT_CALL(stats, Get(Stat::speed)).WillOnce(Return(StatDescriptor(6)));
    Counter counter("test", Stat::speed, Counter::Reset::action, false);
    EXPECT_EQ(counter.ResetTo(stats), 0);
}

}
