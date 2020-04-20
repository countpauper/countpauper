#include "pch.h"
#include "Math.h"

namespace Engine::Test
{


TEST(Math, Gaussian)
{
    EXPECT_EQ(1, Gaussian(0, 1));
    const double HalfWidthHalfMaximum = sqrt(2 * log(2));
    EXPECT_EQ(0.5, Gaussian(HalfWidthHalfMaximum, 1));
    EXPECT_EQ(0.5, Gaussian(-HalfWidthHalfMaximum, 1));
    EXPECT_EQ(0.5, Gaussian(2 * HalfWidthHalfMaximum, 2));
}


}