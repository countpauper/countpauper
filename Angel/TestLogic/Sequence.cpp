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

TEST(TestSequence, Construction)
{
	Sequence empty;
	EXPECT_EQ(empty.size(), 0);

	Element voidElement;
	Sequence voidSequence(std::move(voidElement));
	EXPECT_EQ(voidSequence.size(), 0);


	Sequence single(predicate(L"ginny"));
	EXPECT_EQ(single.size(), 1);

	Sequence cats(predicate(L"ginny"), predicate(L"max"));
	EXPECT_EQ(cats.size(), 2);

	Sequence array(array(id(L"ginny"), id(L"max")));
	EXPECT_EQ(array.size(), 2);

	Knowledge k;
	k.Know(sequence(predicate(L"table"), predicate(L"moon"), predicate(L"hope")));
	EXPECT_EQ(k.Clauses(), 1);
}

}
}
}
