#include <gtest/gtest.h>
#include "Utility/Range.h"
#include "Geometry/Gtest.h"

namespace Engine::Test
{

TEST(Range, Size)
{
    EXPECT_EQ(1, Range(0, 1).Size());
    EXPECT_EQ(0, Range(2, 2).Size());
    EXPECT_FALSE(Range(3, 3));
}

TEST(Range, Middle)
{
    EXPECT_EQ(3, Range<int>(2, 4).Middle());
    EXPECT_EQ(2, Range<int>(2, 3).Middle());
    EXPECT_EQ(1.5, Range<double>(1, 2).Middle());
    EXPECT_EQ(1.5, Range<double>(2, 1).Middle());
}

TEST(Range, MiddleOfEverything)
{
    EXPECT_EQ(0, Range<double>::infinity().Middle());
}

TEST(Range, Union)
{
    EXPECT_EQ(Range(0, 2), Range(0, 1) | Range(1, 2));
    EXPECT_EQ(Range(-1, 2), Range(-1, 0) | Range(1, 2));
    EXPECT_EQ(Range(-1, 2), Range(1, 2) | Range(-1, 0));
}

TEST(Range, Intersection)
{
    EXPECT_FALSE(Range(0, 1) & Range(1, 2));
    EXPECT_EQ(Range(0, 1), Range(-1, 2) & Range(0, 1));
    EXPECT_EQ(Range(0, 1), Range(-1, 1) & Range(0, 3));
    EXPECT_FALSE(Range(2, 3) & Range(0, 1));
}

TEST(Range, Move)
{
    EXPECT_EQ(1, (Range(0, 1) + 1).Size());
}

TEST(Range, Expand)
{
    EXPECT_EQ(2, Range(0, 1).Expand(1).Size());
}

TEST(Range, Cast)
{
    EXPECT_EQ(Range<int>(0, 1), Range<int>(Range<double>(0.1, 1.2)));
    EXPECT_EQ(Range<double>(-1.0, 2.0), Range<double>(Range<int>(-1, 2)));
}

TEST(Range, consts)
{
    EXPECT_EQ(std::numeric_limits<double>::infinity(), Range<double>::infinity().Size());
    EXPECT_TRUE(Range<double>::infinity() | -10000.0);
    EXPECT_EQ(0, Range<double>::null().Size());
    EXPECT_FALSE(bool(Range<double>::null() | 0.0));
    EXPECT_FALSE(Range<double>::empty());
    EXPECT_FALSE(Range<int>::empty());
}

TEST(Range, Inclusion)
{
    EXPECT_TRUE((Range<int>() | 2)[2]);
    EXPECT_TRUE((Range<int>() | -1)[-1]);
    EXPECT_TRUE((Range<int>() | -1 | 2)[1]);
    EXPECT_FALSE((Range<int>() | -1)[2]);
}

TEST(Range, Compare)
{
    EXPECT_TRUE(Range<int>(2,3) > 1 );
    EXPECT_FALSE(Range<int>(2,3) > 2 );
    EXPECT_TRUE(Range<int>(2,4) >= 3 );

    EXPECT_TRUE(Range<int>(2,3) < 4 );
    EXPECT_TRUE(Range<int>(2,3) < 3 );
    EXPECT_FALSE(Range<int>(2,3) < 2 );
    EXPECT_TRUE(Range<int>(2,4) <= 3 );
}

TEST(Range, Numbers)
{
    static constexpr float NaN = std::numeric_limits<float>::quiet_NaN();
    EXPECT_FALSE(Range<float>(NaN, 0).IsNumbers());
    EXPECT_TRUE(Range<float>::empty().IsNumbers());
    EXPECT_EQ(Range<float>(-1.0, NaN).GetNumber(), -1.0);
    EXPECT_EQ(Range<float>(NaN, 1.0).GetNumber(), 1.0);
}
}

