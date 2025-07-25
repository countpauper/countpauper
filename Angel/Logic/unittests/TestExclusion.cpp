#include <gtest/gtest.h>
#include "Logic/Knowledge.h"
#include "Logic/Exclusion.h"
#include "Logic/Expression.h"


namespace Angel::Logic::Test
{

TEST(Exclusion, Construction)
{
    EXPECT_TRUE(Exclusion{}.empty());
    EXPECT_TRUE(Exclusion{Expression()}.empty());
    EXPECT_EQ(Exclusion{False}.size(), 1);
    EXPECT_EQ(Expression(MultiOperator(L'⊕'), {True, False}), (Exclusion{True, False}));   

	static_assert(Logic::is_operation<Exclusion>);
}

TEST(Exclusion, Exclusions)
{
    Knowledge k;
    EXPECT_EQ(k.Infer(Exclusion{}), False);
    EXPECT_EQ(k.Infer(Exclusion{True}), True);
    EXPECT_EQ(k.Infer(Exclusion{True, False}), True);
    EXPECT_EQ(k.Infer(Exclusion{True, True}), False);
}

TEST(Exclusion, Nest)
{
    Knowledge k;
    EXPECT_EQ(k.Infer(Exclusion{Exclusion{False}, Exclusion{True}}), True);
    EXPECT_EQ(k.Infer(Exclusion{Predicate("cat")}), False);
}


TEST(Exclusion, Simplify)
{
    EXPECT_EQ((Exclusion{}).Simplify(), False);
    EXPECT_EQ((Exclusion{Predicate("maybe"), True}).Simplify(), Negation(Predicate("maybe")));
    EXPECT_EQ((Exclusion{Predicate("maybe"), False}).Simplify(), Predicate("maybe"));
    EXPECT_EQ((Exclusion{False, Integer(3)}).Simplify(), Integer(3));
    EXPECT_EQ((Exclusion{Integer(3), False}).Simplify(), Integer(3));
    EXPECT_EQ((Exclusion{True, True}).Simplify(), False);
    EXPECT_EQ((Exclusion{True, False}).Simplify(), True);
    EXPECT_EQ((Exclusion{Id("cat"), All(List{Id("dog"), Id("bunny")})}), 
        (Exclusion{Id("cat"), Id("dog"), Id("bunny")}));

}

TEST(Exclusion, Substitute)
{
	EXPECT_EQ((Exclusion{Id("ginny"), Variable("C")}).Substitute(Conjunction{Equal{Variable("C"), Id("gizmo")}}),
		(Exclusion{Id("ginny"), Id("gizmo")}));
}

TEST(Exclusion, Inference)
{
    Knowledge k;
    k.Know(Predicate("cat"));
    EXPECT_EQ(k.Infer(Exclusion{Predicate("cat")}), True);
}

TEST(Exclusion, InferOneTrueishValue)
{
    Knowledge k;
    EXPECT_EQ(k.Infer(Exclusion{False, Integer(3)}), Integer(3));
    EXPECT_EQ(k.Infer(Exclusion{Integer(0), True}), True);
}


TEST(Exclusion, InferContainer)
{
    Knowledge k;
	List a{Id("ginny"), Id("gizmo")};
	Set b{Id("max"), Id("gizmo")};
	EXPECT_EQ(k.Infer(Exclusion{a, b}), (List{Id("ginny"), Id("max")}));  
	EXPECT_EQ(k.Infer(Exclusion{b, List{}}), b);
	EXPECT_EQ(k.Infer(Exclusion{a, a}), (List{}));
}


TEST(Exclusion, to_string)
{
    EXPECT_EQ(to_string(Exclusion{True}), "true");
    EXPECT_EQ(to_string(Exclusion{True, False}), "true⊕false");
}

}

