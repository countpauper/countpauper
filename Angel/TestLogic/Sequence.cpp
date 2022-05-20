#include "pch.h"
#include "Logic/Sequence.h"
#include "Logic/Knowledge.h"
#include "Logic/Id.h"
#include "Logic/Sequence.h"
#include "Logic/Boolean.h"

namespace Angel::Logic::Test
{

TEST(TestSequence, Construction)
{
	Sequence empty;
	EXPECT_EQ(empty.size(), 0);

	Sequence voidSequence((Object()));	// most vexing parse
	EXPECT_EQ(voidSequence.size(), 0);


	Sequence single(id(L"ginny"));
	EXPECT_EQ(single.size(), 1);
    EXPECT_EQ(single.front(), id(L"ginny"));

	Sequence cats(boolean(L"ginny"), boolean(L"max"));
	EXPECT_EQ(cats.size(), 2);

	Sequence seq(sequence(id(L"ginny"), id(L"max")));
	EXPECT_EQ(seq.size(), 1);
   
    Knowledge k;
	EXPECT_THROW(k.Know(sequence(id(L"table"), id(L"moon"), id(L"hope"))), std::invalid_argument);
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

