#include "format.hpp"
#include <gtest/gtest.h>
#include <gmock/gmock.h>


using ::testing::StrEq;

namespace eul::test
{

TEST(format, float_0)
{
    EXPECT_THAT(to_string(0.0), StrEq("0.0"));
}

TEST(format, float_positive)
{
    EXPECT_THAT(to_string(1.0),  StrEq("1.00000"));
    EXPECT_THAT(to_string(2.1),  StrEq("2.10000"));
    EXPECT_THAT(to_string(1234.56),  StrEq("1234.56"));
    EXPECT_THAT(to_string(0.34), StrEq("0.34000"));
}

TEST(format, float_negative)
{
    EXPECT_THAT(to_string(-0.0), StrEq("0.0"));
    EXPECT_THAT(to_string(-1.0), StrEq("-1.00000"));
    EXPECT_THAT(to_string(-0.056), StrEq("-0.05600"));
    EXPECT_THAT(to_string(-54321), StrEq("-54321.0"));
}

TEST(format, float_special_numbers)
{
    EXPECT_THAT(to_string(NAN), StrEq("NaN"));
    EXPECT_THAT(to_string(INFINITY), StrEq("INF"));
    EXPECT_THAT(to_string(-INFINITY), StrEq("-INF"));

}

TEST(format, float_exponent)
{
    EXPECT_THAT(to_string(1e6), StrEq("1.00000e6"));
    EXPECT_THAT(to_string(1e-12), StrEq("1.00000e-12"));
    EXPECT_THAT(to_string(0.0000123456), StrEq("1.23456e-5"));
    EXPECT_THAT(to_string(FLT_MAX), StrEq("3.40282e38"));
}

}
