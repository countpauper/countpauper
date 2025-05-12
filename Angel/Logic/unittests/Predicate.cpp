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
	EXPECT_EQ(Predicate("cat").Compute(k), Boolean(false));
	k.Know(Predicate("cat"));
	EXPECT_EQ(Predicate("cat").Compute(k), Boolean(true));
	EXPECT_EQ(Predicate("dog").Compute(k), Boolean(false));

}

TEST(TestPredicate, Valence1)
{
	Knowledge k;
	EXPECT_EQ(Predicate("cat", List{Id("ginny")}).Compute(k), Boolean(false));
	k.Know(Predicate("cat", List{Id("ginny")}));
	EXPECT_EQ(Predicate("cat", List{Id("ginny")}).Compute(k), Boolean(true));
	EXPECT_EQ(Predicate("cat", List{Id("woofer")}).Compute(k), Boolean(false));
	EXPECT_EQ(Predicate("dog", List{Id("ginny")}).Compute(k), Boolean(false));
}

}

