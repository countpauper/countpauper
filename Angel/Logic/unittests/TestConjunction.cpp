#include <gtest/gtest.h>
#include "Logic/Knowledge.h"
#include "Logic/Conjunction.h"
#include "Logic/Expression.h"


namespace Angel::Logic::Test
{

TEST(Conjunction, Construction)
{
    EXPECT_TRUE(Conjunction{}.empty());
    EXPECT_TRUE(Conjunction{Expression()}.empty());
    EXPECT_EQ(Conjunction{Boolean(false)}.size(), 1);
    EXPECT_EQ(Expression(BinaryOperator(L'∧'), {Boolean(true), Boolean(false)}), (Conjunction{Boolean(true), Boolean(false)}));   


	static_assert(Logic::IsOperation<Conjunction>);
}

TEST(Conjunction, Conjunctions)
{
    Knowledge k;
    EXPECT_EQ(k.Infer(Conjunction{}), Boolean(true));
    EXPECT_EQ(k.Infer(Conjunction{Boolean(true)}), Boolean(true));
    EXPECT_EQ(k.Infer(Conjunction{Boolean(true), Boolean(false)}), Boolean(false));
    EXPECT_EQ(k.Infer(Conjunction{Conjunction{Boolean(true)}}), Boolean(true));
}

TEST(Conjunction, Nest)
{
    Knowledge k;
    EXPECT_EQ(k.Infer(Conjunction{Conjunction{Boolean(false)}, Conjunction{Boolean(true)}}), Boolean(false));
    EXPECT_EQ(k.Infer(Conjunction{Predicate("cat")}), Boolean(false));
}


TEST(Conjunction, Simplify)
{
    EXPECT_EQ((Conjunction{}).Simplify(), Boolean(true));
    EXPECT_EQ((Conjunction{Predicate("maybe"), Boolean(true)}).Simplify(), Predicate("maybe"));
    EXPECT_EQ((Conjunction{Predicate("maybe"), Boolean(false)}).Simplify(), Boolean(false));
    EXPECT_EQ((Conjunction{Boolean(true), Boolean(true)}).Simplify(), Boolean(true));
    EXPECT_EQ((Conjunction{Boolean(true), Boolean(false)}).Simplify(), Boolean(false));

}

TEST(Conjunction, Substitute)
{
	EXPECT_EQ((Conjunction{Id("ginny"), Variable("C")}).Substitute(Conjunction{Equation{Variable("C"), Id("gizmo")}}),
		(Conjunction{Id("ginny"), Id("gizmo")}));
}

TEST(Conjunction, Inference)
{
    Knowledge k;
    k.Know(Predicate("cat"));
    EXPECT_EQ(k.Infer(Conjunction{Predicate("cat")}), Boolean(true));
}

TEST(Conjunction, to_string)
{
    EXPECT_EQ(to_string(Conjunction{Boolean(true)}), "true");
    EXPECT_EQ(to_string(Conjunction{Boolean(true), Boolean(false)}), "true∧false");
}

}

