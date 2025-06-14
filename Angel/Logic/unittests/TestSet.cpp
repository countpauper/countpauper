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

	// TODO static_assert(Logic::IsContainer<Set>);
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

TEST(Set, Add)
{
	Set a{Id("ginny")};
	EXPECT_EQ(a.Add(Id("ginny")), 0);
	EXPECT_EQ(a.size(), 1);
	EXPECT_EQ(a.Add(Id("gizmo")), 1);
	EXPECT_EQ(a.size(), 2);
}

TEST(Set, Concatenate)
{
	Set a{Id("ginny")};
	Set b{Id("gizmo")};
	EXPECT_EQ(a+b, (Set{Id("ginny"), Id("gizmo")}));
	EXPECT_EQ(a+a, a);
}

TEST(Set, Remove)
{
	Set a{Id("ginny")};
	EXPECT_EQ(a.Remove(Id("gizmo")), 0);
	EXPECT_EQ(a.size(), 1);
	EXPECT_EQ(a.Remove(Id("ginny")), 1);
	EXPECT_EQ(a.size(), 0);
}

TEST(Set, RemoveAssociation)
{
	Set a{Association(Id("ginny"), Integer(2))};
	EXPECT_EQ(a.Remove(Id("gizmo")), 0);
	EXPECT_EQ(a.size(), 1);
	EXPECT_EQ(a.Remove(Association(Id("ginny"), Integer(-2))), 0);
	EXPECT_EQ(a.Remove(Id("ginny")), 1);
	EXPECT_EQ(a.size(), 0);
}

TEST(Set, Delete)
{
	Set a{Id("ginny")};
	Set b{Id("gizmo")};
	EXPECT_EQ(a-b, a);
	EXPECT_EQ(a-a, Set{});
}

TEST(Set, Union)
{
	Set a{Id("ginny"), Id("gizmo")};
	Set b{Id("max"), Id("gizmo")};
	EXPECT_EQ(a|b, (Set{Id("ginny"), Id("gizmo"), Id("max")}));
}

TEST(Set, Intersetion)
{
	Set a{Id("ginny"), Id("gizmo")};
	Set b{Id("max"), Id("gizmo")};
	EXPECT_EQ(a&b, (Set{Id("gizmo")}));
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

TEST(Set, Matches)
{
	EXPECT_EQ((Set{Integer(3)}).Matches(Set{Integer(3)}, {}).Simplify(), Boolean(true));	
	EXPECT_EQ((Set{Integer(2)}).Matches(Set{Integer(4)}, {}).Simplify(), Boolean(false));	
	EXPECT_EQ((Set{Integer(2)}).Matches(Set{Integer(2), Integer(4)}, {}), Boolean(false));
	EXPECT_EQ((Set{Integer(2),Integer(3)}).Matches(Set{Integer(2)}, {}), Boolean(false));	
	EXPECT_EQ((Set{Integer(2)}).Matches(Set{Integer(2), Integer(4)}, {}), Boolean(false));
}

TEST(Set, MatchesVariable)
{
	EXPECT_EQ((Set{Integer(3), Integer(2)}).Matches(Set{Variable("X"), Integer(3)}, {}).Simplify(), 
		(Equation{Integer(2), Variable("X")}));	
	EXPECT_EQ((Set{Tuple("L")}).Matches(Set{Boolean(true), Id("ginny")}, {}).Simplify(), 
		(Equation{Tuple("L"), Set{Boolean(true), Id("ginny")} }));	
	EXPECT_EQ((Set{Integer(0),Tuple("L")}).Matches(Set{Boolean(false), Integer(0)}, {}).Simplify(), 
		(Equation{Tuple("L"), Set{Boolean(false)} }));	
}


TEST(Set, Infer)
{
	Knowledge k;
 	EXPECT_EQ(k.Infer(Set{Summation{Integer(2), Integer(3)}, Integer(5)}), (Set{Integer(5)}));
}

TEST(Set, Get)
{
	EXPECT_EQ(Set().Get(Id("ginny")), Boolean(false));
	EXPECT_EQ((Set{Id("ginny"), Id("ginny")}).Get(Id("ginny")), Boolean(true));
	EXPECT_EQ((Set{
		Association{Id("ginny"), Integer(1)},
		Association{Id("gizmo"), Integer(2)},
		Association{Id("ginny"), Integer(3)}}).Get(Id("ginny")), Integer(3));
}


TEST(Set, to_string)
{
	EXPECT_EQ(to_string(Set{Id("ginny")}), "{ginny}");
	EXPECT_EQ(to_string(Set{Id("ginny"), Id("gizmo")}), "{gizmo,ginny}");
}

}
