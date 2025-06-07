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
}

TEST(Expression, CantCastNull)
{
    EXPECT_THROW(Expression().Cast<Boolean>(), CastException);
    EXPECT_FALSE(bool(Expression().TryCast<Boolean>()));
}


TEST(Expression, GetIfIs)
{
    EXPECT_TRUE(Expression(Boolean(true)).Is<Boolean>());
    EXPECT_EQ(Expression(Boolean(false)).Get<Boolean>(), Boolean(false));
    EXPECT_THROW(Expression(Boolean(false)).Get<Integer>(), std::bad_variant_access);
    EXPECT_EQ(*Expression(Boolean(true)).GetIf<Boolean>(), Boolean(true));
    EXPECT_EQ(Expression(Integer(1)).GetIf<Boolean>(), nullptr);
}

}