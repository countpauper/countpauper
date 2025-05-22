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

TEST(Predicate, Valence0IsNotKnown)
{
	Knowledge k;
	EXPECT_EQ(k.Infer(Predicate("cat")), Boolean(false));
}
	
TEST(Predicate, Valence0IsKnownOrNot)
{
	Knowledge k { Predicate("cat")};
	EXPECT_EQ(k.Infer(Predicate("cat")), Boolean(true));
	EXPECT_EQ(k.Infer(Predicate("dog")), Boolean(false));

}

TEST(Predicate, Valence1isNoTKnown)
{
	Knowledge k;
	EXPECT_EQ(k.Infer(Predicate("cat", List{Id("ginny")})), Boolean(false));
}

TEST(Predicate, Valence1IsKnownOrNot)
{
	Knowledge k;
	k.Know(Predicate("cat", List{Id("ginny")}));
	EXPECT_EQ(k.Infer(Predicate("cat", List{Id("ginny")})), Boolean(true));
	EXPECT_EQ(k.Infer(Predicate("cat", List{Id("woofer")})), Boolean(false));
	EXPECT_EQ(k.Infer(Predicate("dog", List{Id("ginny")})), Boolean(false));
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
	EXPECT_EQ(k.Infer(Predicate("legs", List{Summation{Integer(2), Boolean(false)}})), Boolean(false));
	EXPECT_EQ(k.Infer(Predicate("legs", List{Summation{Integer(2), Integer(2)}})), Boolean(true));
}

}

