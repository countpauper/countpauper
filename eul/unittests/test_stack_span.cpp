#include "stack_span.hpp"
#include <gtest/gtest.h>


namespace eul::Test
{


TEST(stack_span, create)
{
    auto s = stack_span<int>(4);
    s[3] = 1;
    EXPECT_EQ(s[3], 1);
}


TEST(stack_span, empty)
{
    auto s = stack_span<int>(0);
    EXPECT_EQ(s.size(), 0);
}

}
