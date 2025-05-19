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

TEST(Predicate, Valence0)
{
	Knowledge k;
	EXPECT_EQ(Predicate("cat").Infer(k), Boolean(false));
	k.Know(Predicate("cat"));
	EXPECT_EQ(Predicate("cat").Infer(k), Boolean(true));
	EXPECT_EQ(Predicate("dog").Infer(k), Boolean(false));

}

TEST(Predicate, Valence1)
{
	Knowledge k;
	EXPECT_EQ(Predicate("cat", List{Id("ginny")}).Infer(k), Boolean(false));
	k.Know(Predicate("cat", List{Id("ginny")}));
	EXPECT_EQ(Predicate("cat", List{Id("ginny")}).Infer(k), Boolean(true));
	EXPECT_EQ(Predicate("cat", List{Id("woofer")}).Infer(k), Boolean(false));
	EXPECT_EQ(Predicate("dog", List{Id("ginny")}).Infer(k), Boolean(false));
}

TEST(Predicate, to_string)
{
	EXPECT_EQ(to_string(Predicate("gizmo", List{})), "gizmo");
	EXPECT_EQ(to_string(Predicate("cat", List{Id("ginny")})), "cat(ginny)");
}

TEST(Predicate, Infer)
{
	Knowledge k;
	k.Know(Predicate("legs", List{Integer(4)}));
	EXPECT_EQ(Predicate("legs", List{Summation{Integer(2), Boolean(false)}}).Infer(k), Boolean(false));
	EXPECT_EQ(Predicate("legs", List{Summation{Integer(2), Integer(2)}}).Infer(k), Boolean(true));
}

}

