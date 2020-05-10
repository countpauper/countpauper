#include "pch.h"
#include "Maths.h"

namespace Engine::Test
{


TEST(Math, Gaussian)
{
    EXPECT_EQ(1, Gaussian(0, 1));
    const double HalfWidthHalfMaximum = FullWidthHalfMaximum(1.0);
    EXPECT_EQ(0.5, Gaussian(HalfWidthHalfMaximum, 1));
    EXPECT_EQ(0.5, Gaussian(-HalfWidthHalfMaximum, 1));
    EXPECT_EQ(0.5, Gaussian(2 * HalfWidthHalfMaximum, 2));
}

TEST(Math, Average)
{
    EXPECT_EQ(0.5, Average(0, 1));
    EXPECT_EQ(0, Average(-1, 1));
    EXPECT_EQ(2, Average(3, 1));
    EXPECT_EQ(5.6, Average(5.6, 5.6));
    // Nan and infinite not yet defined
}

TEST(Math, LinearInterpolotion)
{
    EXPECT_EQ(1.0, Lerp(1.0, 2.0, 0.0));
    EXPECT_EQ(2.0, Lerp(1.0, 2.0, 1.0));
    EXPECT_EQ(2.0, Lerp(2.0, 1.0, 0.0));
    EXPECT_EQ(1.0, Lerp(1.0, 1.0, 0.12345));
    EXPECT_EQ(1.5, Lerp(1.0, 2.0, 0.5));
    EXPECT_EQ(1.5, Lerp(2.0, 1.0, 0.5));
    EXPECT_EQ(-1, Lerp(-3.0, 1.0, 0.5));
}

// TODO: infinite doesn't work as expected but specific checks would cost performance

TEST(Math, FloatInterpolation)
{
    EXPECT_EQ(1.0f, Lerp(1.0f, 2.0f, 0.0));
    EXPECT_EQ(2.0f, Lerp(1.0f, 2.0f, 1.0));
    EXPECT_EQ(1.5f, Lerp(1.0f, 2.0f, 0.5));
    EXPECT_EQ(-1.0f, Lerp(-3.0f, 1.0f, 0.5));
}

TEST(Math, LinearExtrapolation)
{
    EXPECT_EQ(3.0, Lerp(1.0, 2.0, 2.0));
    EXPECT_EQ(0.0, Lerp(1.0, 2.0, -1.0));
}

}