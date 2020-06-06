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

TEST(Math, Sigmoid)
{
    EXPECT_EQ(0.5, Sigmoid(0));
    EXPECT_LT(Sigmoid(-1.0), 0.5);
    EXPECT_GT(Sigmoid(1.0), 0.5);
    EXPECT_EQ(1.0, Sigmoid(std::numeric_limits<double>::infinity()));
    EXPECT_EQ(0.0, Sigmoid(-std::numeric_limits<double>::infinity()));
    EXPECT_NEAR(1, Sigmoid(1e6), 1e-12);
    EXPECT_NEAR(0, Sigmoid(-1e6), 1e-12);
}


TEST(Math, Bilinear)
{
    double data[] = { 1.0, 2.0, 3.0, 4.0 };
    EXPECT_EQ(1.0, BilinearInterpolation<double>(data, 0, 0));
    EXPECT_EQ(2.0, BilinearInterpolation<double>(data, 0, 1));
    EXPECT_EQ(3.0, BilinearInterpolation<double>(data, 1, 0));
    EXPECT_EQ(4.0, BilinearInterpolation<double>(data, 1, 1));
    EXPECT_EQ(2.5, BilinearInterpolation<double>(data, 0.5, 0.5));
}


TEST(Math, Trilinear)
{
    double data[] = { 1.0, -1.0, 2.0, -2.0, 3.0, -3.0, 4.0, -4.0 };
    EXPECT_EQ(1.0, TrilinearInterpolation<double>(data, Vector(0, 0, 0)));
    EXPECT_EQ(2.0, TrilinearInterpolation<double>(data, Vector(0, 1, 0)));
    EXPECT_EQ(3.0, TrilinearInterpolation<double>(data, Vector(1, 0, 0)));
    EXPECT_EQ(4.0, TrilinearInterpolation<double>(data, Vector(1, 1, 0)));
    EXPECT_EQ(2.5, TrilinearInterpolation<double>(data, Vector(0.5, 0.5, 0)));
    EXPECT_EQ(-1.0, TrilinearInterpolation<double>(data, Vector(0, 0, 1)));
    EXPECT_EQ(-2.0, TrilinearInterpolation<double>(data, Vector(0, 1, 1)));
    EXPECT_EQ(-3.0, TrilinearInterpolation<double>(data, Vector(1, 0, 1)));
    EXPECT_EQ(-4.0, TrilinearInterpolation<double>(data, Vector(1, 1, 1)));
    EXPECT_EQ(-2.5, TrilinearInterpolation<double>(data, Vector(0.5, 0.5, 1)));
    EXPECT_EQ(0, TrilinearInterpolation<double>(data, Vector(0.5, 0.5, 0.5)));
}


TEST(Math, ALittleMoreInt)
{
    EXPECT_GT(ALittleMore<int>(0), 0);
    EXPECT_LE(ALittleMore<int>(0), 1);

    EXPECT_GT(ALittleMore<int>(100), 100);
    EXPECT_LE(ALittleMore<int>(100), 101);
    EXPECT_GT(ALittleMore<int>(-1000), -1000);

    EXPECT_THROW(ALittleMore<int>(std::numeric_limits<int>::max()), std::exception);
}


TEST(Math, ALittleMoreDouble)
{
    EXPECT_GT(ALittleMore<double>(0), 0);
    EXPECT_LE(ALittleMore<double>(0), std::numeric_limits<double>::epsilon());

    EXPECT_GT(ALittleMore<double>(1e6), 1e6);
    EXPECT_LE(ALittleMore<double>(1e6), 1e6 + 1.0);
    EXPECT_GT(ALittleMore<double>(-1e3), -1e3);

    EXPECT_THROW(ALittleMore<double>(std::numeric_limits<double>::max()), std::exception);
    EXPECT_THROW(ALittleMore<double>(std::numeric_limits<double>::infinity()), std::exception);
}

}