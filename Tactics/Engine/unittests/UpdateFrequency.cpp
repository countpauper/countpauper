#include <gtest/gtest.h>
#include "Utility/UpdateFrequency.h"

namespace Engine::Test
{

TEST(UpdateFrequency, Positive)
{
    EXPECT_THROW(UpdateFrequency([]() {}, -1.0), std::invalid_argument);
}

TEST(UpdateFrequency, Postpone)
{
    unsigned count = 0;
    UpdateFrequency update([&count]() { count++; }, 1);
    update.Tick(0);
    EXPECT_EQ(count, 0);
    update.Tick(0.5);
    EXPECT_EQ(count, 0);
    update.Tick(0.5);
    EXPECT_EQ(count, 1);
}

TEST(UpdateFrequency, Multiple)
{
    unsigned count = 0;
    UpdateFrequency update([&count]() { count++; }, 10);
    update.Tick(0.5);
    EXPECT_EQ(count, 5);
}

TEST(UpdateFrequency, Remainder)
{
    unsigned count = 0;
    UpdateFrequency update([&count]() { count++; }, 2);
    update.Tick(0.7);
    EXPECT_EQ(count, 1);
    update.Tick(0.7);
    EXPECT_EQ(count, 2);
    update.Tick(0.7);
    EXPECT_EQ(count, 4);
}

TEST(UpdateFrequency, Never)
{
    unsigned count = 0;
    UpdateFrequency update([&count]() { count++; }, 0);
    update.Tick(1000.0);
    update.Tick(1e15);
    EXPECT_EQ(count, 0);
}

} // Engine::Test
