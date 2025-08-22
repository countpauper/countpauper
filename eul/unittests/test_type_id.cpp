#include "type_id.hpp"
#include <gtest/gtest.h>

namespace eul::Test
{


TEST(TypeId, Unique)
{
    EXPECT_EQ(type_id<int>(), type_id<int>());
    EXPECT_NE(type_id<int>(), type_id<unsigned>());
}

TEST(TypeId, ConstIsSameType)
{
    EXPECT_EQ(type_id<int>(), type_id<const int>());
}

TEST(TypeId, ReferenceIsSametype)
{
    EXPECT_EQ(type_id<int&>(), type_id<int>());
}

TEST(TypeId, Struct)
{
    struct Test 
    {
        int a;
        bool b;
    };
    EXPECT_NE(type_id<Test>(), 0);
}

TEST(TypeId, OfValue)
{
    const int a=0;
    EXPECT_EQ(type_id(a), type_id<int>());
};

TEST(TypeId, VolatileIsSame)
{
    volatile int a=0;
    EXPECT_EQ(type_id(a), type_id<int>());
};

}
