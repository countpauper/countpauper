#include "pch.h"
#include "Logic/Set.h"
#include "Logic/Knowledge.h"
#include "Logic/Id.h"
#include "Logic/Sequence.h"
#include "Logic/Boolean.h"

namespace Angel::Logic::Test
{

TEST(TestSet, Construction)
{
	Set empty;
	EXPECT_EQ(empty.size(), 0);

	Set voidSet{ Object() };
	EXPECT_EQ(voidSet.size(), 0);


	Set single(id("ginny"));
	EXPECT_EQ(single.size(), 1);

	Set cats(id("ginny"), id("max"));
	EXPECT_EQ(cats.size(), 2);

	Set seq_set(sequence(id("ginny"), id("max")));
	EXPECT_EQ(seq_set.size(), 1);

	Knowledge k;
    EXPECT_THROW(k.Know(set(id("table"), id("moon"), id("hope"))), std::invalid_argument);
}


TEST(TestSet, Sequence)
{
	Sequence array{ set(id("cat")), set(id("dog")) };
	EXPECT_EQ(2, array.size());
	EXPECT_NE(array, Sequence(id("cat"), id("dog")));
}


TEST(TestSet, Compare)
{
	Set a(id("ginny"));
	Set b(id("ginny"));
	EXPECT_EQ(a, b);

	Set aa(id("ginny"), id("ginny"));
	EXPECT_EQ(a, aa);

	Set ab(id("ginny"), id("max"));
	Set ba(id("max"), id("ginny"));
	EXPECT_EQ(ab, ba);
	EXPECT_NE(a, ab);
	EXPECT_NE(b, ba);

}

}
