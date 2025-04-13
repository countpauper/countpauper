#include <gtest/gtest.h>
#include "Logic/Knowledge.h"
#include "Logic/Predicate.h"
#include "Logic/Sequence.h"

namespace Angel::Logic::Test
{

TEST(TestPredicate, 0ary)
{
	Knowledge k;
	EXPECT_FALSE(k.Query(predicate("cat")).Trivial());
	k.Know(predicate("cat"));
	EXPECT_TRUE(k.Query(predicate("cat")).Trivial());
	EXPECT_FALSE(k.Query(predicate("dog")).Trivial());
}

TEST(TestPredicate, 1ary)
{
	Knowledge k;
	EXPECT_FALSE(k.Query(predicate("cat", Sequence(id("ginny")))).Trivial());
	k.Know(predicate("cat", Sequence(id("ginny"))));
	EXPECT_TRUE(k.Query(predicate("cat", Sequence(id("ginny")))).Trivial());
	EXPECT_FALSE(k.Query(predicate("cat", Sequence(id("woofer")))).Trivial());
	EXPECT_FALSE(k.Query(predicate("dog", Sequence(id("ginny")))).Trivial());
}

}

