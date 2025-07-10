#include "Logic/Expression.h"
#include <gtest/gtest.h>

namespace Angel::Logic::Test
{

TEST(Expression, DefaultExpressionIsNull)
{
    EXPECT_FALSE(Expression());
    EXPECT_EQ(Expression(), Expression());
    EXPECT_EQ(to_string(Expression()), "null");
    ASSERT_TRUE(Expression().IsConstant());
    static_assert(sizeof(Expression)<=72);
}

TEST(Expression, CantCastNull)
{
    EXPECT_THROW(Expression().Cast<Boolean>(), CastException);
    EXPECT_FALSE(bool(Expression().TryCast<Boolean>()));
}


TEST(Expression, GetIfIs)
{
    EXPECT_TRUE(Expression(True).Is<Boolean>());
    EXPECT_EQ(Expression(False).Get<Boolean>(), False);
    EXPECT_THROW(Expression(False).Get<Integer>(), std::bad_variant_access);
    EXPECT_EQ(*Expression(True).GetIf<Boolean>(), True);
    EXPECT_EQ(Expression(Integer(1)).GetIf<Boolean>(), nullptr);
}

}