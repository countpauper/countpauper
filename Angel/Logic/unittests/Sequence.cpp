#include <gtest/gtest.h>
#include "Logic/Sequence.h"
#include "Logic/Knowledge.h"
#include "Logic/Id.h"
#include "Logic/Sequence.h"
#include "Logic/Boolean.h"

namespace Angel::Logic::Test
{
/*
TEST(TestSequence, Construction)
{
	Sequence empty;
	EXPECT_EQ(empty.size(), 0);
	EXPECT_FALSE(empty);

	Sequence voidSequence((Object()));	// most vexing parse
	EXPECT_EQ(voidSequence.size(), 0);


	Sequence single(id("ginny"));
	EXPECT_TRUE(single);
	EXPECT_EQ(single.size(), 1);
    EXPECT_EQ(single.front(), id("ginny"));

	Sequence cats(id("ginny"), id("max"));
	EXPECT_EQ(cats.size(), 2);

	Sequence seq(sequence(id("ginny"), id("max")));
	EXPECT_EQ(seq.size(), 1);
   
    Knowledge k;
	EXPECT_THROW(k.Know(sequence(id("table"), id("moon"), id("hope"))), std::invalid_argument);
}


TEST(TestSequence, Compare)
{

	Sequence a(id("ginny"));
	Sequence b(id("ginny"));
	EXPECT_EQ(a, b);

	Sequence ab(id("ginny"), id("max"));
	Sequence ba(id("max"), id("ginny"));
	EXPECT_NE(ab, ba);
}
	*/


}

