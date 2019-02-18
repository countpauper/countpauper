#include "pch.h"
#include "Logic/Sequence.h"
#include "Logic/Knowledge.h"
#include "Logic/Id.h"
#include "Logic/Array.h"
#include "Logic/Boolean.h"

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

	Sequence voidSequence((Element()));	// most vexing parse
	EXPECT_EQ(voidSequence.size(), 0);


	Sequence single(id(L"ginny"));
	EXPECT_EQ(single.size(), 1);

	Sequence cats(boolean(L"ginny"), boolean(L"max"));
	EXPECT_EQ(cats.size(), 2);

	Sequence array(array(id(L"ginny"), id(L"max")));
	EXPECT_EQ(array.size(), 2);

	Knowledge k;
	k.Know(sequence(id(L"table"), id(L"moon"), id(L"hope")));
	EXPECT_EQ(k.Clauses(), 1);
}


TEST(TestSequence, Compare)
{

	Sequence a(id(L"ginny"));
	Sequence b(id(L"ginny"));
	EXPECT_EQ(a, b);

	Sequence ab(id(L"ginny"), id(L"max"));
	Sequence ba(id(L"max"), id(L"ginny"));
	EXPECT_NE(ab, ba);
}

}
}
}
