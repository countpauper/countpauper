#include <gtest/gtest.h>
#include "Utility/FixedPoint.h"

namespace Engine::Test
{

TEST(FixedPoint, Float)
{
    EXPECT_EQ(static_cast<float>(FixedPoint(0.5f)), 0.5f);
    EXPECT_EQ(static_cast<float>(FixedPoint(-0.5f)), -0.5f);
}

TEST(FixedPoint, Double)
{
    EXPECT_DOUBLE_EQ(static_cast<double>(FixedPoint(0.125)), 0.125);
    EXPECT_DOUBLE_EQ(static_cast<double>(FixedPoint(-0.125)), -0.125);
}

TEST(FixedPoint, Integral)
{
    EXPECT_DOUBLE_EQ(static_cast<int>(FixedPoint(10)), 10);
    EXPECT_DOUBLE_EQ(static_cast<int>(FixedPoint(-1)), -1);
}

TEST(FixedPoint, Unsigned)
{
    EXPECT_EQ(static_cast<float>(FixedPoint<16,unsigned>(0.5f)), 0.5f);
    EXPECT_THROW((FixedPoint<16,unsigned>(-0.5f)), std::range_error);
}

TEST(FixedPoint, OutOfRange)
{
    EXPECT_THROW((FixedPoint<8,int16_t>(1e3)), std::range_error);
    EXPECT_THROW((FixedPoint<30,int32_t>(-5.0f)), std::range_error);
    EXPECT_THROW(FixedPoint(65536), std::range_error);
}

TEST(FixedPoint, FractionOnly)
{
    EXPECT_EQ(static_cast<float>(FixedPoint<8,uint8_t>(0.125)), 0.125);
    EXPECT_EQ(static_cast<float>(FixedPoint<7,int8_t>(-0.125)), -0.125);
}

TEST(FixedPoint, Rescale)
{
    EXPECT_EQ(static_cast<FixedPoint<8>>(FixedPoint<16>(0.125)), FixedPoint<16>(0.125));
    EXPECT_EQ(static_cast<FixedPoint<4>>(FixedPoint<8,int8_t>(0.125)), FixedPoint<4>(0.125));
}

TEST(FixedPoint, 64bit)
{
    EXPECT_EQ(static_cast<double>(FixedPoint<32,uint64_t>(0.03125)), 0.03125);
    EXPECT_EQ(static_cast<double>(FixedPoint<31,int64_t>(-0.03125)), -0.03125);
}

TEST(FixedPoint, Arithmatic)
{
    EXPECT_EQ(FixedPoint(10) + FixedPoint(0.5), FixedPoint(10.5));
    EXPECT_EQ(FixedPoint(8) - FixedPoint(0.25), FixedPoint(7.75));
    EXPECT_EQ(FixedPoint(10) * FixedPoint(0.5), FixedPoint(5));
    EXPECT_EQ(FixedPoint(10) / FixedPoint(2), FixedPoint(5));
}

TEST(FixedPoint, Compare)
{
    EXPECT_GT(FixedPoint(8), FixedPoint(6));
    EXPECT_GE(FixedPoint(8), FixedPoint(6));
    EXPECT_GE(FixedPoint(6), FixedPoint(6));
    EXPECT_LT(FixedPoint(0.5), FixedPoint(6));
    EXPECT_LE(FixedPoint(0.5), FixedPoint(6));
    EXPECT_LE(FixedPoint(0.5), FixedPoint(0.5));
}

}
