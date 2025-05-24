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
    EXPECT_EQ(Disjunction{Boolean(false)}.size(), 1);    
    EXPECT_EQ(Expression(BinaryOperator(L'|'), {Boolean(true), Boolean(false)}), (Disjunction{Boolean(true), Boolean(false)}));   

	static_assert(Logic::IsOperation<Disjunction>);
}

TEST(Disjunction, Conjunctions)
{
    Knowledge k;
    EXPECT_EQ(k.Infer(Disjunction{}), Boolean(false));
    EXPECT_EQ(k.Infer(Disjunction{Boolean(true)}), Boolean(true));
    EXPECT_EQ(k.Infer(Disjunction{Boolean(true), Boolean(false)}), Boolean(true));
    EXPECT_EQ(k.Infer(Disjunction{Disjunction{Boolean(true)}}), Boolean(true));
    EXPECT_EQ(k.Infer(Disjunction{Disjunction{Boolean(false), Integer(3), Integer(2)}}), Integer(3));
}

TEST(Disjunction, Nest)
{
    Knowledge k;
    EXPECT_EQ(k.Infer(Disjunction{Disjunction{Boolean(false)}, Disjunction{Boolean(true)}}), Boolean(true));
    EXPECT_EQ(k.Infer(Disjunction{Predicate("cat")}), Boolean(false));
}

TEST(Disjunction, Simplify)
{
    EXPECT_EQ((Disjunction{}).Simplify(), Boolean(false));
    EXPECT_EQ((Disjunction{Boolean(true)}).Simplify(), Boolean(true));
}

TEST(Disjunction, Substitute)
{
	EXPECT_EQ((Disjunction{Integer(3), Variable("B")}).Substitute(Conjunction{Equation{Variable("B"), Boolean(false)}}),
		(Disjunction{Integer(3), Boolean(false)}));
}

TEST(Disjunction, Inference)
{
    Knowledge k;
    k.Know(Predicate("cat"));
    EXPECT_EQ(k.Infer(Disjunction{Predicate("cat")}), Boolean(true));
}

TEST(Disjunction, to_string)
{
    EXPECT_EQ(to_string(Disjunction{Boolean(true)}), "true");
    EXPECT_EQ(to_string(Disjunction{Boolean(true), Boolean(false)}), "true|false");
}

}

