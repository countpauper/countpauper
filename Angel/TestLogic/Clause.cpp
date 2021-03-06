#include "pch.h"
#include "Logic/Knowledge.h"
#include "Logic/Predicate.h"
#include "Logic/Sequence.h"
#include "Logic/Clause.h"

namespace Angel
{
	namespace Logic
	{
		namespace Test
		{

			TEST(TestClause, Trivial)
			{
				Knowledge k;
				k.Know(clause(Predicate(L"cat")));
				EXPECT_TRUE(k.Query(predicate(L"cat")));
				EXPECT_FALSE(k.Query(predicate(L"dog")));
			}

			TEST(TestClause, Condition)
			{
				Knowledge k;
				k.Know(clause(Predicate(L"cat"), Array(predicate(L"ginny"))));
				EXPECT_FALSE(k.Query(predicate(L"cat")));
				k.Know(predicate(L"ginny"));
				EXPECT_TRUE(k.Query(predicate(L"cat")));
			}

			TEST(TestClause, Conjunction)
			{
				Knowledge k;
				k.Know(clause(Predicate(L"cat"), Array(predicate(L"fuzzy"), predicate(L"noisy"))));
				k.Know(clause(Predicate(L"hamster"), Array(predicate(L"fuzzy"), predicate(L"quiet"))));
				EXPECT_FALSE(k.Query(predicate(L"cat")));
				EXPECT_FALSE(k.Query(predicate(L"hamster")));
				k.Know(predicate(L"fuzzy"));
				k.Know(predicate(L"noisy"));
				EXPECT_TRUE(k.Query(predicate(L"cat")));
				EXPECT_FALSE(k.Query(predicate(L"hamster")));
			}

			TEST(TestClause, Predicate1Ary)
			{
				Knowledge k;
				k.Know(clause(Predicate(L"cat", Sequence(id(L"ginny"))),
					Array(predicate(L"fuzzy", Sequence(id(L"ginny"))))));
				k.Know(clause(Predicate(L"cat", Sequence(id(L"woofer"))),
					Array(predicate(L"fuzzy", Sequence(id(L"woofer"))))));
				EXPECT_FALSE(k.Query(predicate(L"cat", Sequence(id(L"ginny")))));
				k.Know(predicate(L"fuzzy", Sequence(id(L"ginny"))));

				EXPECT_TRUE(k.Query(predicate(L"cat", Sequence(id(L"ginny")))));
				EXPECT_FALSE(k.Query(predicate(L"cat", Sequence(id(L"woofer")))));
			}

			TEST(TestClause, Predicate2Ary)
			{
				Knowledge k;
				k.Know(clause(Predicate(L"cats", Sequence(id(L"ginny"), id(L"max"))),
					Array(predicate(L"cat", Sequence(id(L"ginny"))),
						predicate(L"cat", Sequence(id(L"max"))))));
				EXPECT_FALSE(k.Query(predicate(L"cats", Sequence(id(L"ginny"), id(L"max")))));

				k.Know(predicate(L"cat", Sequence(id(L"ginny"))));
				k.Know(predicate(L"cat", Sequence(id(L"max"))));

				EXPECT_TRUE(k.Query(predicate(L"cats", Sequence(id(L"ginny"), id(L"max")))));
			}


		}
	}
}
