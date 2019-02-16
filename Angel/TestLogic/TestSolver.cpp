#include "pch.h"
#include "Logic/Knowledge.h"
#include "Logic/Predicate.h"
#include "Logic/Sequence.h"
#include "Logic/Boolean.h"
#include "Logic/Clause.h"

namespace Angel
{
namespace Logic 
{
namespace Test
{

TEST(TestSolver, Predicate0)
{
	Knowledge k;
	EXPECT_FALSE(k.Query(predicate(L"cat")));
	k.Know(predicate(L"cat"));
	EXPECT_TRUE(k.Query(predicate(L"cat")));
	EXPECT_FALSE(k.Query(predicate(L"dog")));
}

TEST(TestSolver, Predicate1)
{
	Knowledge k;
	EXPECT_FALSE(k.Query(predicate(L"cat", sequence(id(L"ginny")))));
	k.Know(predicate(L"cat", sequence(id(L"ginny"))));
	EXPECT_TRUE(k.Query(predicate(L"cat", sequence(id(L"ginny")))));
	EXPECT_FALSE(k.Query(predicate(L"cat", sequence(id(L"woofer")))));
	EXPECT_FALSE(k.Query(predicate(L"dog", sequence(id(L"ginny")))));
}

TEST(TestSolver, Boolean)
{
	Knowledge k;
	EXPECT_TRUE(k.Query(boolean(true)));
	EXPECT_FALSE(k.Query(boolean(false)));
}

TEST(TestSolver, TestSequence)
{
	Sequence empty;
	EXPECT_EQ(empty.size(), 0);

	Sequence single(predicate(L"ginny"));
	EXPECT_EQ(single.size(), 1);

	Sequence cats(predicate(L"ginny"), predicate(L"max"));
	EXPECT_EQ(cats.size(), 2);

	Knowledge k;
	k.Know(sequence(predicate(L"table"), predicate(L"moon"), predicate(L"hope")));
	EXPECT_EQ(k.Clauses(), 1);
}

TEST(TestSolver, TrivialClause)
{
	Knowledge k;
	k.Know(clause(Predicate(L"cat"), Sequence()));
	EXPECT_TRUE(k.Query(predicate(L"cat")));
}

TEST(TestSolver, PredicateClause)
{
	Knowledge k;
	k.Know(clause(Predicate(L"cat"), Sequence(predicate(L"ginny"))));
	EXPECT_FALSE(k.Query(predicate(L"cat")));
	k.Know(predicate(L"ginny"));
	EXPECT_TRUE(k.Query(predicate(L"cat")));
}



}
}
}
