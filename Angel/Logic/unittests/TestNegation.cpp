#include <gtest/gtest.h>
#include "Logic/Knowledge.h"
#include "Logic/Negation.h"
#include "Logic/Expression.h"

namespace Angel::Logic::Test
{

TEST(Negation, Construction)
{
    EXPECT_TRUE(bool(Negation(False)));
    EXPECT_EQ(Negation(True), Negation(True));
    EXPECT_NE(Negation(True), Negation(False));
    EXPECT_EQ(Expression(PrefixOperator(L'¬'), {False}), Negation(False));
	static_assert(sizeof(Negation)<=24);
}

TEST(Negation, Simplify)
{
    EXPECT_EQ(Negation(Expression(Negation(Conjunction{False}))).Simplify(), False);
    EXPECT_EQ(Negation(Disjunction{False}).Simplify(), True);
}

TEST(Negation, Substitute)
{
	EXPECT_EQ(Negation(Variable("P")).Substitute(Conjunction{Equal{Variable("P"), Integer(3)}}),
		Negation(Integer(3)));
}

TEST(Negation, Matches)
{
    EXPECT_EQ(Negation(True).Matches(False, {}).Simplify(), True);
    EXPECT_EQ(Negation(False).Matches(False, {}).Simplify(), False);
    EXPECT_EQ(Negation(Variable("B")).Matches(False, {}).Simplify(), (Equal{Variable("B"), True}));
    EXPECT_EQ(Negation(True).Matches(Variable("B"), {}).Simplify(), (Equal{False, Variable("B")}));
}

TEST(Negation, Negations)
{
    Knowledge k;
    EXPECT_EQ(k.Infer(Negation(False)), True);
    EXPECT_EQ(k.Infer(Negation(Integer(2))), False);
    EXPECT_EQ(k.Infer(Negation(Integer(0))), True);
}

TEST(Negation, to_string)
{
    EXPECT_EQ(to_string(Negation(True)), "¬true");
    EXPECT_EQ(to_string(Negation(Integer(-2))), "¬-2");
}

}

