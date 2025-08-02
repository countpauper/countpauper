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
    EXPECT_EQ(Operator::Find<BinaryOperator>("."), BinaryOperator("."));
}

TEST(Operator, ConstructMulti)
{
    EXPECT_TRUE(MultiOperator(L'+'));
    EXPECT_EQ(MultiOperator(L'∧'), MultiOperator("&"));
    EXPECT_NE(MultiOperator(L'-'), PrefixOperator(L'-'));
    EXPECT_EQ(MultiOperator(L'⋅').Operands(), 3);
    EXPECT_EQ((Operator::Find<BinaryOperator, MultiOperator>("*")), MultiOperator(L'⋅'));
    EXPECT_EQ((Operator::Find<MultiOperator, BinaryOperator>("⋅")), MultiOperator(L'⋅'));
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
    EXPECT_NE(PostfixOperator(L'²'), PostfixOperator("³"));
    EXPECT_EQ(PostfixOperator(L'-').Operands(), 1);
    static_assert(sizeof(PostfixOperator)==4);
}

TEST(Operator, ConstructOrder)
{
    EXPECT_TRUE(Order(L'>'));
    EXPECT_EQ(Order(L'≥'), Order(">="));
    EXPECT_NE(Order(L'<'), Order("<="));
    EXPECT_EQ(Order(L'=').Operands(), 3);
    EXPECT_TRUE(Order(L'<').IsComparator());
    static_assert(sizeof(Order)==4);
}

TEST(Operator, String)
{
    EXPECT_EQ(std::string(MultiOperator(L'&')), "&");
    EXPECT_EQ(std::string(MultiOperator("!=")), "≠");
}

TEST(Operator, Description)
{
    EXPECT_EQ(MultiOperator(L'∨').Description(), "or");
    EXPECT_EQ(Filter("@").Description(), "any of");
}

TEST(Operator, Operate)
{
    EXPECT_EQ((MultiOperator{L'+'})(Integer(1), Integer(2)), Integer(3));
    EXPECT_TRUE(Order(L'=')(Integer(1), Integer(1)));
}

TEST(Operator, RemarkIsNotAnOperator)
{
    EXPECT_THROW(PrefixOperator("#"), std::invalid_argument);
    EXPECT_THROW(BinaryOperator("#"), std::invalid_argument);
    EXPECT_THROW(MultiOperator("#"), std::invalid_argument);
    EXPECT_THROW(Comparator("#"), std::invalid_argument);
}

TEST(Operator, Precedence)
{
    EXPECT_EQ(MultiOperator{L'+'}.Precedence(), MultiOperator{L'-'}.Precedence()); 
    EXPECT_EQ(MultiOperator{L'÷'}.Precedence(), MultiOperator{L'⋅'}.Precedence()); 
    // value of highest precedence. Take the value to operate on 
    EXPECT_GT(PrefixOperator{L'$'}.Precedence(), PostfixOperator{L'²'}.Precedence()); 
    // PE before MD
    EXPECT_LT(MultiOperator{L'⋅'}.Precedence(), PostfixOperator{L'²'}.Precedence()); 
    // MD before AS
    EXPECT_LT(MultiOperator{L'+'}.Precedence(), MultiOperator{L'⋅'}.Precedence()); 
    // Mathtmetical operators before logical operators
    EXPECT_GT(MultiOperator{L'+'}.Precedence(), MultiOperator{L'⊕'}.Precedence()); 
    // Everything before clause, the operation is the antecedent
    EXPECT_GT(MultiOperator{L'∨'}.Precedence(), BinaryOperator{L'←'}.Precedence()); 
}


std::string op2str(Operator op, std::string lhs, std::string rhs, bool braces)
{
    std::string unbraced;
    if (op.Operands()==1)
    {
        if (lhs.size() == 1)
            std::swap(lhs, rhs);

        if (op.IsPrefix())
            unbraced = std::string(op) + lhs;
        else
            unbraced = lhs + std::string(op);
    }
    else 
    {
        unbraced = lhs +  std::string(op) + rhs;
    }
    if (braces)
        return std::format("({})", unbraced);
    else 
        return unbraced;
}

TEST(Operator, DISABLED_PrintPrecedence)
{
    auto operators = Operator::All();
    for(const auto& first_op : operators) 
    {
        for(const auto& second_op : operators) 
        {
            if (first_op.Precedence() == second_op.Precedence())
            {
                std::cout << "= " << op2str(second_op,op2str(first_op, "X", "Y",false), "Z", false) << std::endl;

            }
            else if (first_op.Precedence() > second_op.Precedence())
            {
                std::cout <<  "> " << op2str(second_op,op2str(first_op, "X", "Y", true), "Z", false)<< std::endl;
            }
            else 
            {
                std::cout << "< " << op2str(first_op, "X", op2str(second_op, "Y", "Z", true), false)<< std::endl;
            }
        }
    }
}

}
