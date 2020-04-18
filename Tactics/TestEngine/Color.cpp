#include "pch.h"
#include <math.h>
#include "Color.h"

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
    EXPECT_EQ(100, color.r);
    EXPECT_EQ(200, color.g);
    EXPECT_EQ(255, color.b);
    EXPECT_EQ(40, color.a);

    RGBA half_color = color * 0.5;
    EXPECT_EQ(50, half_color.r);
    EXPECT_EQ(100, half_color.g);
    EXPECT_EQ(128, half_color.b);
    EXPECT_EQ(40, half_color.a);

    RGBA no_color = color * -0.1;
    EXPECT_EQ(0, no_color.r);
    EXPECT_EQ(0, no_color.g);
    EXPECT_EQ(0, no_color.b);
    EXPECT_EQ(40, no_color.a);
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
