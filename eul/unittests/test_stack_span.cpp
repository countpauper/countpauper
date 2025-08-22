#include "stack_span.hpp"
#include <gtest/gtest.h>


namespace eul::Test
{


TEST(StackSpan, Create)
{
    auto s = StackSpan<int>(4);
    s[3] = 1;
    EXPECT_EQ(s[3], 1);
}


TEST(StackSpan, Empty)
{
    auto s = StackSpan<int>(0);
    EXPECT_EQ(s.size(), 0);
}

}
