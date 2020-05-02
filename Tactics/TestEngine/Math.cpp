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


}