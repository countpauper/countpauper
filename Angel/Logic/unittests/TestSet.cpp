#include <gtest/gtest.h>
#include "Logic/Set.h"
#include "Logic/Knowledge.h"
#include "Logic/Id.h"
#include "Logic/List.h"
#include "Logic/Boolean.h"

namespace Angel::Logic::Test
{

TEST(Set, Construction)
{
	Set empty;
	EXPECT_EQ(empty.size(), 0);
	EXPECT_FALSE(empty);


	Set single{Id("ginny")};
	EXPECT_EQ(single.size(), 1);
	EXPECT_TRUE(single);
	
	Set cats{Id("ginny"), Id("max")};
	EXPECT_EQ(cats.size(), 2);


	Set seq_set({List{Id("ginny"), Id("max")}});
	EXPECT_EQ(seq_set.size(), 1);

}

TEST(Set, List)
{
	List list{ Set({Id("cat")}), Set({Id("dog")}) };
	EXPECT_EQ(list.size(), 2);
	EXPECT_NE(list, List({Id("cat"), Id("dog")}));
}

TEST(Set, Compare)
{
	Set a{Id("ginny")};
	Set b{Id("ginny")};
	EXPECT_EQ(a, b);

	Set aa{Id("ginny"), Id("ginny")};
	EXPECT_EQ(a, aa);

	Set ab{Id("ginny"), Id("max")};
	Set ba{Id("max"), Id("ginny")};
	EXPECT_EQ(ab, ba);
	EXPECT_NE(a, ab);
	EXPECT_NE(b, ba);
}


TEST(Set, Infer)
{
	Knowledge k;
 	EXPECT_EQ(k.Infer(Set{Summation{Integer(2), Integer(3)}, Integer(5)}), (Set{Integer(5)}));
}

}
