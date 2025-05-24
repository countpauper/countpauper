#include <gtest/gtest.h>
#include "Logic/Set.h"
#include "Logic/Knowledge.h"
#include "Logic/Expression.h"

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

	Set association_set{Association{Id("ginny"), Id("cat")}, 
		Association{Id("ginny"), Id("dog")}};
	EXPECT_EQ(association_set.size(), 1);

	// TODO: should be a set static_assert(Logic::IsContainer<Set>);
}

TEST(Set, List)
{
	List list{ Set({Id("cat")}), Set({Id("dog")}) };
	EXPECT_EQ(list.size(), 2);
	EXPECT_NE(list, List({Id("cat"), Id("dog")}));
}

TEST(Set, Iteration)
{
	Set cats{Id("ginny")};
	EXPECT_EQ(*cats.begin(), Id("ginny"));
	EXPECT_NE(cats.begin(), cats.end());
	auto it = cats.begin();
	++it;
	EXPECT_EQ(it, cats.end());

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

TEST(Set, Simplify)
{
    EXPECT_EQ((Set{}).Simplify(), Set());
    EXPECT_EQ((Set{Association(Id("ginny"), Conjunction{})}).Simplify(), (Set{Id("ginny")}));
}

TEST(Set, Substitute)
{
	EXPECT_EQ((Set{Boolean(true), Variable("I")}).Substitute(Conjunction{Equation{Variable("I"), Integer(3)}}),
		(Set{Boolean(true), Integer(3)}));
}

TEST(Set, Infer)
{
	Knowledge k;
 	EXPECT_EQ(k.Infer(Set{Summation{Integer(2), Integer(3)}, Integer(5)}), (Set{Integer(5)}));
}

}
