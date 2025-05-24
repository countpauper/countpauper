#include "Logic/Expression.h"
#include <gtest/gtest.h>

namespace Angel::Logic::Test
{

TEST(Expression, DefaultExpressionIsNull)
{
    EXPECT_FALSE(Expression());
    EXPECT_EQ(Expression(), Expression());
    EXPECT_EQ(to_string(Expression()), "null");
}

TEST(Expression, CantCastNull)
{
    EXPECT_THROW(Expression().Cast<Boolean>(), CastException);
    EXPECT_FALSE(bool(Expression().TryCast<Boolean>()));
}

}