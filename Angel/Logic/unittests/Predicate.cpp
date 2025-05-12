#include <gtest/gtest.h>
#include "Logic/Knowledge.h"
#include "Logic/Predicate.h"
#include "Logic/List.h"

namespace Angel::Logic::Test
{

TEST(TestPredicate, Construction)
{
	EXPECT_THROW(Predicate(""), std::invalid_argument);
	EXPECT_NE(Predicate("test"), Predicate("Test"));
}

TEST(TestPredicate, Valence0)
{
	Knowledge k;
	EXPECT_EQ(k.Query(Predicate("cat")), Boolean(false));
	k.Know(Predicate("cat"));
	EXPECT_EQ(k.Query(Predicate("cat")), Boolean(true));
	EXPECT_EQ(k.Query(Predicate("dog")), Boolean(false));

}

TEST(TestPredicate, Valence1)
{
	Knowledge k;
	EXPECT_EQ(k.Query(Predicate("cat", List{Id("ginny")})), Boolean(false));
	k.Know(Predicate("cat", List{Id("ginny")}));
	EXPECT_EQ(k.Query(Predicate("cat", List{Id("ginny")})), Boolean(true));
	EXPECT_EQ(k.Query(Predicate("cat", List{Id("woofer")})), Boolean(false));
	EXPECT_EQ(k.Query(Predicate("dog", List{Id("ginny")})), Boolean(false));
}

}

