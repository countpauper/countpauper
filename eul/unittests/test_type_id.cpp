#include "type_id.hpp"
#include <gtest/gtest.h>

namespace eul::Test
{


TEST(type_id, unique)
{
    EXPECT_EQ(type_id<int>(), type_id<int>());
    EXPECT_NE(type_id<int>(), type_id<unsigned>());
}

TEST(type_id, const_is_same_type)
{
    EXPECT_EQ(type_id<int>(), type_id<const int>());
}

TEST(type_id, reference_is_same_type)
{
    EXPECT_EQ(type_id<int&>(), type_id<int>());
}

TEST(type_id, struct)
{
    struct Test 
    {
        int a;
        bool b;
    };
    EXPECT_NE(type_id<Test>(), 0);
}

TEST(type_id, of_value)
{
    const int a=0;
    EXPECT_EQ(type_id(a), type_id<int>());
};

TEST(type_id, volatile_is_same_type_id)
{
    volatile int a=0;
    EXPECT_EQ(type_id(a), type_id<int>());
};

}
