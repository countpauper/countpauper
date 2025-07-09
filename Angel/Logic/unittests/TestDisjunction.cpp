#include <gtest/gtest.h>
#include "Logic/Knowledge.h"
#include "Logic/Disjunction.h"
#include "Logic/Expression.h"


namespace Angel::Logic::Test
{

TEST(Disjunction, Construction)
{
    EXPECT_TRUE(Disjunction{}.empty());
    EXPECT_TRUE(Disjunction{Expression()}.empty());
    EXPECT_EQ(Disjunction{False}.size(), 1);    
    EXPECT_EQ(Expression(MultiOperator(L'∨'), {True, False}), (Disjunction{True, False}));   

	static_assert(Logic::is_operation<Disjunction>);
}

TEST(Disjunction, Conjunctions)
{
    Knowledge k;
    EXPECT_EQ(k.Infer(Disjunction{}), False);
    EXPECT_EQ(k.Infer(Disjunction{True}), True);
    EXPECT_EQ(k.Infer(Disjunction{True, False}), True);
    EXPECT_EQ(k.Infer(Disjunction{Disjunction{True}}), True);
    EXPECT_EQ(k.Infer(Disjunction{Disjunction{False, Integer(3), Integer(2)}}), Integer(3));
}

TEST(Disjunction, Nest)
{
    Knowledge k;
    EXPECT_EQ(k.Infer(Disjunction{Disjunction{False}, Disjunction{True}}), True);
    EXPECT_EQ(k.Infer(Disjunction{Predicate("cat")}), False);
}

TEST(Disjunction, Simplify)
{
    EXPECT_EQ((Disjunction{}).Simplify(), False);
    EXPECT_EQ((Disjunction{True}).Simplify(), True);
    EXPECT_EQ((Disjunction{Predicate("maybe"), False}).Simplify(), Predicate("maybe"));
    EXPECT_EQ((Disjunction{Predicate("maybe"), True}).Simplify(), True);
    EXPECT_EQ((Disjunction{False, False}).Simplify(), False);
    EXPECT_EQ((Disjunction{True, False}).Simplify(), True);

}

TEST(Disjunction, Substitute)
{
	EXPECT_EQ((Disjunction{Integer(3), Variable("B")}).Substitute(Conjunction{Equal{Variable("B"), False}}),
		(Disjunction{Integer(3), False}));
}

TEST(Disjunction, Inference)
{
    Knowledge k;
    k.Know(Predicate("cat"));
    EXPECT_EQ(k.Infer(Disjunction{Predicate("cat")}), True);
}

TEST(Disjunction, InferContainer)
{
    Knowledge k;
	Set a{Id("ginny"), Id("gizmo")};
	List b{Id("max"), Id("gizmo")};
	EXPECT_EQ(k.Infer(Disjunction{a, b}), (Set{Id("max"), Id("gizmo"), Id("ginny")}));
	EXPECT_EQ(k.Infer(Disjunction{b, a}), (List{Id("max"), Id("gizmo"), Id("ginny")}));
}

TEST(Disjunction, to_string)
{
    EXPECT_EQ(to_string(Disjunction{True}), "true");
    EXPECT_EQ(to_string(Disjunction{True, False}), "true∨false");
}

}

