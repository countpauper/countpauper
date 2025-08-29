#include "copy_counter.hpp"

#include <gtest/gtest.h>

namespace eul::Test 
{

TEST(copy_counter, construction_destruction)
{
    copy_count count; 
    {
        copy_counter a(count);
        copy_counter b(count);
        EXPECT_EQ(a->current(), 2);
    }
    EXPECT_EQ(count.current(), 0);
    EXPECT_EQ(count.constructed, 2);
    EXPECT_EQ(count.destructed, 2);
}

TEST(copy_counter, copy)
{
    copy_count count; 
    {
        copy_counter a(count);
        copy_counter b(a);
        copy_counter c;
        c = a;
        EXPECT_EQ(a->current(), 3);
    }
    EXPECT_EQ(count.current(), 0);
    EXPECT_EQ(count.constructed, 1);
    EXPECT_EQ(count.copied, 2);
    EXPECT_EQ(count.destructed, 3);
}


TEST(copy_counter, move)
{
    copy_count count; 
    {
        copy_counter a(count);
        copy_counter b(std::move(a));
        EXPECT_FALSE(a);
        EXPECT_EQ(b->current(), 1);
        copy_counter c;
        c = std::move(a);
        EXPECT_FALSE(c);
        c = std::move(b);
        EXPECT_FALSE(b);
        EXPECT_EQ(c->current(), 1);
    }
    EXPECT_EQ(count.current(), 0);
    EXPECT_EQ(count.moved, 2);
    EXPECT_EQ(count.destructed, 1);
}

TEST(copy_counter, equal)
{
    copy_count count; 
    {
        copy_counter a(count);
        EXPECT_TRUE(a);
        copy_counter b(a);
        EXPECT_TRUE(b);
        EXPECT_EQ(a, b);
        copy_counter c;
        EXPECT_NE(a, c);
        c = b;
        EXPECT_EQ(b, c);
        EXPECT_EQ(a, c);
    }

}


}