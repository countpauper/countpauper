#include "pch.h"
#include "Logic/Knowledge.h"
#include "Logic/Predicate.h"
#include "Logic/Sequence.h"

namespace Angel
{
namespace Logic
{
namespace Test
{

TEST(TestPredicate, 0ary)
{
	Knowledge k;
	EXPECT_FALSE(k.Query(predicate(L"cat")));
	k.Know(predicate(L"cat"));
	EXPECT_TRUE(k.Query(predicate(L"cat")));
	EXPECT_FALSE(k.Query(predicate(L"dog")));
}

TEST(TestPredicate, 1ary)
{
	Knowledge k;
	EXPECT_FALSE(k.Query(predicate(L"cat", sequence(id(L"ginny")))));
	k.Know(predicate(L"cat", sequence(id(L"ginny"))));
	EXPECT_TRUE(k.Query(predicate(L"cat", sequence(id(L"ginny")))));
	EXPECT_FALSE(k.Query(predicate(L"cat", sequence(id(L"woofer")))));
	EXPECT_FALSE(k.Query(predicate(L"dog", sequence(id(L"ginny")))));
}


}
}
}
