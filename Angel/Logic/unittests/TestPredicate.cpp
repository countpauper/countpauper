#include <gtest/gtest.h>
#include "Logic/Knowledge.h"
#include "Logic/Predicate.h"
#include "Logic/Expression.h"

namespace Angel::Logic::Test
{

TEST(Predicate, Construction)
{
	EXPECT_THROW(Predicate(""), std::invalid_argument);
	EXPECT_NE(Predicate("test"), Predicate("Test"));
}

TEST(Predicate, Valence0IsFalseIfNothingKnown)
{
	Knowledge k;
	EXPECT_EQ(k.Infer(Predicate("cat")), Boolean(false));
}
	
TEST(Predicate, Valence0IsTrueIfKown)
{
	Knowledge k { Predicate("cat")};
	EXPECT_EQ(k.Infer(Predicate("cat")), Boolean(true));
}

TEST(Predicate, Valence0IsFalseIfUnknown)
{
	Knowledge k { Predicate("cat")};
	EXPECT_EQ(k.Infer(Predicate("dog")), Boolean(false));
}

TEST(Predicate, Valence1isNotKnown)
{
	Knowledge k;
	EXPECT_EQ(k.Infer(Predicate("cat", List{Id("ginny")})), Boolean(false));
}

TEST(Predicate, Valence1IsTrueIfKnown)
{
	Knowledge k { Predicate("cat", List{Id("ginny")}) };
	EXPECT_EQ(k.Infer(Predicate("cat", List{Id("ginny")})), Boolean(true));
}

TEST(Predicate, Valence1IsFalsePredicateUnknown)
{
	Knowledge k { Predicate("cat", List{Id("ginny")}) };
	EXPECT_EQ(k.Infer(Predicate("dog", List{Id("ginny")})), Boolean(false));
}

TEST(Predicate, Valence1IsFalseIfArgumentUnknown)
{
	Knowledge k { Predicate("cat", List{Id("ginny")}) };
	EXPECT_EQ(k.Infer(Predicate("cat", List{Id("woofer")})), Boolean(false));
}

TEST(Predicate, to_string)
{
	EXPECT_EQ(to_string(Predicate("gizmo", List{})), "gizmo");
	EXPECT_EQ(to_string(Predicate("cat", List{Id("ginny")})), "cat(ginny)");
}

TEST(Predicate, Substitute)
{
	EXPECT_EQ(Predicate("cat", {Variable("C")}).Substitute(Conjunction{Equation{Variable("C"), Id("ginny")}}),
		Predicate("cat", {Id("ginny")}));
}

TEST(Predicate, Infer)
{
	Knowledge k;
	k.Know(Predicate("legs", List{Integer(4)}));
	EXPECT_EQ(k.Infer(Predicate("legs", List{Summation{Integer(2), Boolean(false)}})), Boolean(false));
	EXPECT_EQ(k.Infer(Predicate("legs", List{Summation{Integer(2), Integer(2)}})), Boolean(true));
}

}

