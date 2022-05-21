#include "pch.h"
#include "Logic/Knowledge.h"
#include "Logic/Predicate.h"
#include "Logic/Sequence.h"

namespace Angel::Logic::Test
{

TEST(TestPredicate, 0ary)
{
	Knowledge k;
	EXPECT_FALSE(k.Query(predicate(L"cat")).Trivial());
	k.Know(predicate(L"cat"));
	EXPECT_TRUE(k.Query(predicate(L"cat")).Trivial());
	EXPECT_FALSE(k.Query(predicate(L"dog")).Trivial());
}

TEST(TestPredicate, 1ary)
{
	Knowledge k;
	EXPECT_FALSE(k.Query(predicate(L"cat", Sequence(id(L"ginny")))).Trivial());
	k.Know(predicate(L"cat", Sequence(id(L"ginny"))));
	EXPECT_TRUE(k.Query(predicate(L"cat", Sequence(id(L"ginny")))).Trivial());
	EXPECT_FALSE(k.Query(predicate(L"cat", Sequence(id(L"woofer")))).Trivial());
	EXPECT_FALSE(k.Query(predicate(L"dog", Sequence(id(L"ginny")))).Trivial());
}

}

