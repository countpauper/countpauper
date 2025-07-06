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
    EXPECT_EQ(Conjunction{False}.size(), 1);
    EXPECT_EQ(Expression(BinaryOperator(L'∧'), {True, False}), (Conjunction{True, False}));   

	static_assert(Logic::is_operation<Conjunction>);
}

TEST(Conjunction, Conjunctions)
{
    Knowledge k;
    EXPECT_EQ(k.Infer(Conjunction{}), True);
    EXPECT_EQ(k.Infer(Conjunction{True}), True);
    EXPECT_EQ(k.Infer(Conjunction{True, False}), False);
    EXPECT_EQ(k.Infer(Conjunction{Conjunction{True}}), True);
}

TEST(Conjunction, Nest)
{
    Knowledge k;
    EXPECT_EQ(k.Infer(Conjunction{Conjunction{False}, Conjunction{True}}), False);
    EXPECT_EQ(k.Infer(Conjunction{Predicate("cat")}), False);
}


TEST(Conjunction, Simplify)
{
    EXPECT_EQ((Conjunction{}).Simplify(), True);
    EXPECT_EQ((Conjunction{Predicate("maybe"), True}).Simplify(), Predicate("maybe"));
    EXPECT_EQ((Conjunction{Predicate("maybe"), False}).Simplify(), False);
    EXPECT_EQ((Conjunction{True, True}).Simplify(), True);
    EXPECT_EQ((Conjunction{True, False}).Simplify(), False);
    EXPECT_EQ((Conjunction{Id("cat"), All(List{Id("dog"), Id("bunny")})}), 
        (Conjunction{Id("cat"), Id("dog"), Id("bunny")}));

}

TEST(Conjunction, Substitute)
{
	EXPECT_EQ((Conjunction{Id("ginny"), Variable("C")}).Substitute(Conjunction{Equal{Variable("C"), Id("gizmo")}}),
		(Conjunction{Id("ginny"), Id("gizmo")}));
}

TEST(Conjunction, Inference)
{
    Knowledge k;
    k.Know(Predicate("cat"));
    EXPECT_EQ(k.Infer(Conjunction{Predicate("cat")}), True);
}

TEST(Conjunction, InferContainer)
{
    Knowledge k;
	List a{Id("ginny"), Id("gizmo")};
	List b{Id("max"), Id("gizmo")};
	EXPECT_EQ(k.Infer(Conjunction{b, a}), (List{Id("gizmo")}));
	EXPECT_EQ(k.Infer(Conjunction{a, List{}, b}), (List{}));
}


TEST(Conjunction, to_string)
{
    EXPECT_EQ(to_string(Conjunction{True}), "true");
    EXPECT_EQ(to_string(Conjunction{True, False}), "true∧false");
}

}

