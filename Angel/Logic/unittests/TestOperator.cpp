#include "Logic/Operator.h"
#include "Logic/Expression.h"
#include <gtest/gtest.h>

namespace Angel::Logic::Test
{


TEST(Operator, Noperator)
{
    EXPECT_FALSE(Operator().Id());
    EXPECT_EQ(Operator().Operands(), 0);
}

TEST(Operator, ConstructBinary)
{
    EXPECT_TRUE(BinaryOperator(L'.'));
    EXPECT_EQ(BinaryOperator(L'←').Operands(), 2);
}

TEST(Operator, ConstructMulti)
{
    EXPECT_TRUE(MultiOperator(L'+'));
    EXPECT_EQ(MultiOperator(L'∧'), MultiOperator("&"));
    EXPECT_NE(MultiOperator(L'-'), PrefixOperator(L'-'));
    EXPECT_EQ(MultiOperator(L'⋅').Operands(), 3);

    static_assert(sizeof(MultiOperator)==4);
}

TEST(Operator, ConstrucPrefix)
{
    EXPECT_TRUE(PrefixOperator(L'+'));
    EXPECT_EQ(PrefixOperator(L'¬'), PrefixOperator("~"));
    EXPECT_NE(PrefixOperator(L'√'), PrefixOperator("~"));
    EXPECT_EQ(PrefixOperator(L'-').Operands(), 1);

    static_assert(sizeof(PrefixOperator)==4);
}

TEST(Operator, ConstrucPostfix)
{
    EXPECT_TRUE(PostfixOperator(L'+'));
    EXPECT_NE(PostfixOperator(L'²'), PostfixOperator("!"));
    EXPECT_EQ(PostfixOperator(L'-').Operands(), 1);
    static_assert(sizeof(PostfixOperator)==4);
}

TEST(Operator, ConstrucComparator)
{
    EXPECT_TRUE(Comparator(L'>'));
    EXPECT_EQ(Comparator(L'≥'), Comparator(">="));
    EXPECT_NE(Comparator(L'<'), Comparator("<="));
    EXPECT_EQ(Comparator(L'=').Operands(), 3);
    EXPECT_TRUE(Comparator(L'<').IsComparator());
    static_assert(sizeof(Comparator)==4);
}

TEST(Operator, String)
{
    EXPECT_EQ(std::string(MultiOperator(L'&')), "&");
    EXPECT_EQ(std::string(MultiOperator("!=")), "≠");
}

TEST(Operator, Description)
{
    EXPECT_EQ(MultiOperator(L'∨').Description(), "or");
    EXPECT_EQ(PrefixOperator("@").Description(), "any of");
}

TEST(Operator, Operate)
{
    EXPECT_EQ((MultiOperator{L'+'})(Integer(1), Integer(2)), Integer(3));
    EXPECT_TRUE(Comparator(L'=')(Integer(1), Integer(1)));
}


}
