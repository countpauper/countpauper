#include <gtest/gtest.h>
#include "Logic/Knowledge.h"
#include "Logic/Internal/GenericOperation.h"
#include "Logic/Expression.h"

namespace Angel::Logic::Test
{

TEST(NewSummation, Construction)
{
    EXPECT_TRUE(NewSummation({}).empty());
    EXPECT_EQ(NewSummation({Integer(3)}), NewSummation({Integer(3)}));
    EXPECT_NE(NewSummation({Integer(3)}), NewSummation({Integer(3), Integer(2)}));
    EXPECT_EQ(GenericOperation(L'+', {Integer(-2), Integer(-1)}), NewSummation({Integer(-2), Integer(-1)}));   
}

TEST(NewSummation, NewSummations)
{
    Knowledge k;
    EXPECT_EQ(k.Infer(NewSummation({Integer(2)})), Integer(2));
    EXPECT_EQ(k.Infer(NewSummation({Integer(2), Integer(3)})), Integer(5));
    EXPECT_EQ(k.Infer(NewSummation({Integer(2), Integer(-3)})), Integer(-1));
    EXPECT_EQ(k.Infer(NewSummation({True, Integer(2)})), Integer(3));
    EXPECT_EQ(k.Infer(NewSummation({NewSummation({Integer(2)})})), Integer(2));
}

TEST(NewSummation, Simplify)
{
    EXPECT_EQ((NewSummation({})).Simplify(), Integer(0));
    EXPECT_EQ((NewSummation({Integer(2)})).Simplify(), Integer(2));
    EXPECT_EQ((NewSummation({Integer(3), Integer(3)})).Simplify(), Integer(6));
    EXPECT_EQ((NewSummation({Integer(1), All(List{Integer(-2), Integer(3)})})).Simplify(), Integer(2));
}

TEST(NewSummation, Substitute)
{
	EXPECT_EQ((NewSummation({Integer(-2), Variable("I")})).Substitute(Conjunction{Equal{Variable("I"), Integer(3)}}),
		(NewSummation({Integer(-2), Integer(3)})));
}

TEST(NewSummation, Match)
{
	EXPECT_EQ((NewSummation({Integer(1), Integer(2)})).Matches(Integer(3), {}).Simplify(), True);
	EXPECT_EQ((NewSummation({Variable("One"), Integer(2)})).Matches(Integer(3), {Equal{Variable("One"), Integer(1)}}).Simplify(), True);
//	EXPECT_EQ((NewSummation({Variable("One"), Integer(2)})).Matches(Integer(3), {}).Simplify(), (Equal{Variable("One"), Integer(1)}));
}

TEST(NewSummation, MatchArgument)
{
	EXPECT_EQ(Predicate("sum", {NewSummation({Integer(1), Integer(2)})}).Matches(
                Predicate("sum", {Integer(3)}),{}).Simplify(), True);
	EXPECT_EQ(Predicate("sum", {Integer(-4)}).Matches(
                Predicate("sum", {NewSummation({Integer(-5), True})}),{}).Simplify(), True);
}

TEST(NewSummation, Inference)
{
    Knowledge k { Association(Predicate("cat"), Integer(2)) };
    EXPECT_EQ(k.Infer(NewSummation({Integer(4), Predicate("cat")})), Integer(6));
}

TEST(NewSummation, InferContainer)
{
    Knowledge k { Association(Predicate("cat"), List{Id("ginny")}) };
	List b{Id("ginny"), Id("gizmo")};    
	EXPECT_EQ(k.Infer(NewSummation({b, Predicate("cat")})), (List{Id("ginny"), Id("gizmo"), Id("ginny")}));    
}

TEST(NewSummation, to_string)
{
    EXPECT_EQ(to_string(NewSummation({Integer(-3)})), "-3");
    EXPECT_EQ(to_string(NewSummation({Integer(-2), Integer(3)})), "-2+3");
    EXPECT_EQ(to_string(NewSummation({Integer(2), Integer(-3)})), "2-3");
    EXPECT_EQ(to_string(NewSummation({Integer(1), Multiplication{Integer(2), Integer(3)}})), "1+2⋅3");    
}

}

