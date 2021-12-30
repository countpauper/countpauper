#include "pch.h"
#include <math.h>
#include "Color.h"
#include "Maths.h"

namespace Engine::Test
{

TEST(Color, Channels)
{
    RGBA color(1, 2, 3, 4);
    EXPECT_EQ(1, color.r);
    EXPECT_EQ(2, color.g);
    EXPECT_EQ(3, color.b);
    EXPECT_EQ(4, color.a);

    RGBA color32(0x05060708);
    EXPECT_EQ(8, color32.r);
    EXPECT_EQ(7, color32.g);
    EXPECT_EQ(6, color32.b);
    EXPECT_EQ(5, color32.a);
}

TEST(Color, Multiply)
{
    RGBA color(10, 20, 30, 40);

    color *= 10.0;
    EXPECT_EQ(color, RGBA(100, 200, 255, 40));

    EXPECT_EQ(color * 0.5, RGBA(50, 100,128, 40));

    RGBA no_color = color * -0.1;
    EXPECT_EQ(color*-0.1, RGBA(0,0,0,40));
}

TEST(Color, Add)
{
    EXPECT_EQ(RGBA(100, 0, 0) + RGBA(0, 100, 0), RGBA(100, 100, 0));
}

TEST(Color, Lerp)
{
    EXPECT_EQ(Lerp(RGBA(100, 0, 0), RGBA(0, 100, 0), 0.5), RGBA(50, 50, 0));
    EXPECT_EQ(Lerp(RGBA(100, 0, 0, 128), RGBA(0, 100, 0, 128), 0.5), RGBA(50, 50, 0));
    EXPECT_EQ(Lerp(RGBA(100, 0, 0, 100), RGBA(0, 100, 0, 100), 0.5), RGBA(50, 50, 0, 200));
}

TEST(Color, Translucent)
{
    RGBA color(10, 20, 30);
    EXPECT_EQ(color.a, 255);

    RGBA translucent = color.Translucent(0.5);
    EXPECT_EQ(10, translucent.r);
    EXPECT_EQ(128, translucent.a);

    RGBA opaque = color.Translucent(100.0);
    EXPECT_EQ(255, opaque.a);

    RGBA transparent = color.Translucent(-2.0);
    EXPECT_EQ(0, transparent.a);
}

}
