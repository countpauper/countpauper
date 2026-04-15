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
    EXPECT_THROW(FixedPoint(std::numeric_limits<float>::infinity()), std::range_error);
    EXPECT_THROW(FixedPoint(std::numeric_limits<float>::infinity()), std::range_error);
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
    EXPECT_EQ(-FixedPoint(8), FixedPoint(-8));
    EXPECT_EQ(-FixedPoint(-0.01), FixedPoint(0.01));
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

TEST(FixedPoint, Absolute)
{
    EXPECT_EQ(abs(FixedPoint(-3.0)), FixedPoint(3.0));
    EXPECT_EQ(abs(FixedPoint(0.1)), FixedPoint(0.1));
}

TEST(FixedPoint, Modulo)
{
    int i = 0;
    EXPECT_EQ(modfp(FixedPoint(1.2), i), FixedPoint(0.2));
    EXPECT_EQ(i, 1);

    EXPECT_EQ(modfp(FixedPoint(2.0), i), FixedPoint(0));
    EXPECT_EQ(i, 2);


    EXPECT_EQ(modfp(FixedPoint(-4.8), i), FixedPoint(-0.8));
    EXPECT_EQ(i, -4);

    EXPECT_EQ(modfp(FixedPoint(-8), i), FixedPoint(0));
    EXPECT_EQ(i, -8);
}

TEST(FixedPoint, Round)
{
    EXPECT_EQ(round(FixedPoint(2.0)), 2);
    EXPECT_EQ(round(FixedPoint(1.5)), 2);
    EXPECT_EQ(round(FixedPoint(1.4)), 1);
    EXPECT_EQ(round(FixedPoint<0>(3.7)), 4);  // .7 is already rounded on construction
}

TEST(FixedPoint, RoundNegative)
{
    EXPECT_EQ(round(FixedPoint(-2.0)), -2);
    EXPECT_EQ(round(FixedPoint(-2.4)), -2);
    EXPECT_EQ(round(FixedPoint(-2.5)), -3);
    EXPECT_EQ(round(FixedPoint(-2.6)), -3);
}

TEST(FixedPoint, Floor)
{
    EXPECT_EQ(floor(FixedPoint(1.9)), 1);
    EXPECT_EQ(floor(FixedPoint(2.0)), 2);
    EXPECT_EQ(floor(FixedPoint(-2.4)), -3);
}

TEST(FixedPoint, Ceil)
{
    EXPECT_EQ(ceil(FixedPoint(1.9)), 2);
    EXPECT_EQ(ceil(FixedPoint(2.0)), 2);
    EXPECT_EQ(ceil(FixedPoint(-1.0)), -1);
    EXPECT_EQ(ceil(FixedPoint(-2.4)), -2);

}

TEST(FixedPoint, lerp)
{
    EXPECT_EQ(lerp(FixedPoint(1), FixedPoint(3), 0.0f), FixedPoint(1));
    EXPECT_EQ(lerp(FixedPoint(1), FixedPoint(3), 1.0f), FixedPoint(3));
    EXPECT_EQ(lerp(FixedPoint(1), FixedPoint(3), 0.5f), FixedPoint(2));
    EXPECT_EQ(lerp(FixedPoint(1), FixedPoint(3), 1.5f), FixedPoint(4));
    EXPECT_EQ(lerp(FixedPoint(1), FixedPoint(3), -1.0f), FixedPoint(-1));

}
}
