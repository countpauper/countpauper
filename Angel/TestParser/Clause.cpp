#include "pch.h"
#include "Parser/Parser.h"
#include "Logic/Predicate.h"
#include "Logic/Sequence.h"
#include "Logic/Boolean.h"

#pragma warning(disable:4566)	// google test can't represent unicode

namespace Angel
{
namespace Parser
{
namespace Test
{

TEST(TestClause, Predicate)
{
	Logic::Knowledge k = Parse(L"cat(ginny) ginny");

	EXPECT_EQ(k.Clauses(), 2);
	EXPECT_TRUE(k.Knows(Logic::predicate(L"cat", Logic::sequence())));
	EXPECT_TRUE(k.Knows(Logic::id(L"ginny")));
	EXPECT_FALSE(k.Knows(Logic::predicate(L"dog", Logic::sequence())));
}

}
}
}

