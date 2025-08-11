#include <gtest/gtest.h>
#include "Logic/List.h"
#include "Logic/Knowledge.h"
#include "Logic/Expression.h"

namespace Angel::Logic::Test
{

TEST(List, Construction)
{
	List empty;
	EXPECT_EQ(empty.size(), 0);
	EXPECT_FALSE(empty);

	List single{Id("ginny")};
	EXPECT_TRUE(single);
	EXPECT_EQ(single.size(), 1);
    EXPECT_EQ(single.front(), Id("ginny"));

	List cats{Id("ginny"), Id("max")};
	EXPECT_EQ(cats.size(), 2);

	List seq{List{Id("ginny"), Id("max")}};
	EXPECT_EQ(seq.size(), 1);

	static_assert(Logic::is_container<List>);
	static_assert(sizeof(List)<=24);
}


TEST(List, Compare)
{
	List a{Id("ginny")};
	List b{Id("ginny")};
	EXPECT_EQ(a, b);

	List ab{Id("ginny"), Id("max")};
	List ba{Id("max"), Id("ginny")};
	EXPECT_NE(ab, ba);
	List aa{Id("ginny"), Id("ginny")};
	EXPECT_NE(a,aa);
	EXPECT_NE(aa,a);
}


TEST(List, Simplify)
{
    EXPECT_EQ((List{}).Simplify(), List());
    EXPECT_EQ((List{Conjunction{}}).Simplify(), (List{True}));
}

TEST(List, Substitute)
{
	EXPECT_EQ((List{True, Variable("I")}).Substitute(Conjunction{Equal{Variable("I"), Integer(3)}}),
		(List{True, Integer(3)}));
}

TEST(List, SubstiteAll)
{
	EXPECT_EQ((List{True, All("T")}).Substitute(Conjunction{
		Equal{Variable("T"), List{Integer(2), Id("three")}}}),
		(List{True, Integer(2), Id("three")}));
}

TEST(List, SubstiteAllFiltered)
{
	List original{Integer(0), Association(All("T"), LesserEqual{Integer(2)})};
	Hypothesis substitution{Equal{Variable("T"), List{True, Integer(2), Integer(3)}}};
	EXPECT_EQ(original.Substitute(substitution).Simplify(),
		(List{Integer(0), True, Integer(2)}));
}

TEST(List, Matches)
{
	EXPECT_EQ((List{Integer(3)}).Matches(List{Integer(3)}, {}).Simplify(), True);	
	EXPECT_EQ((List{Integer(2)}).Matches(List{Integer(4)}, {}).Simplify(), False);	
	EXPECT_EQ((List{Integer(2)}).Matches(List{Integer(2), Integer(4)}, {}), False);
	EXPECT_EQ((List{Integer(2),Integer(3)}).Matches(List{Integer(2)}, {}), False);	
}


TEST(List, MatchesVariable)
{
	EXPECT_EQ((List{Integer(2),Integer(3)}).Matches(List{Variable("X"), Integer(3)}, {}).Simplify(), 
		(Equal{Variable("X"), Integer(2)}));	
	EXPECT_EQ((List{All("L")}).Matches(List{True, Id("ginny")}, {}).Simplify(), 
		(Equal{List{True, Id("ginny")}, Variable("L")}));	
	EXPECT_EQ((List{Integer(0),All("L")}).Matches(List{Integer(0), False}, {}).Simplify(), 
		(Equal{List{False}, Variable("L")}));	
}
TEST(List, Infer)
{
	Knowledge k;
	EXPECT_EQ(k.Infer(List{Summation({Integer(2), Integer(3)})}), (List{Integer(5)}));
}

TEST(List, Get)
{
	EXPECT_EQ(List().Get(Id("ginny")), Integer(0));
	EXPECT_EQ((List{Id("ginny"), Id("ginny")}).Get(Id("ginny")), Integer(2));
	EXPECT_EQ((List{
		Association{Id("ginny"), Integer(1)},
		Association{Id("gizmo"), Integer(2)},
		Association{Id("ginny"), Integer(3)},
		Id("ginny")}).Get(Id("ginny")), (List{Integer(1), Integer(3), True}));
}



TEST(List, Add)
{
	List a{Id("ginny")};
	EXPECT_EQ(a.Add(Id("ginny")), 1);
	EXPECT_EQ(a.size(), 2);
	EXPECT_EQ(a.Add(Id("gizmo")), 1);
	EXPECT_EQ(a.size(), 3);
}

TEST(List, Concatenate)
{
	List a{Id("ginny")};
	List b{Id("ginny"), Id("gizmo")};
	EXPECT_EQ(a+b, (List{Id("ginny"), Id("ginny"), Id("gizmo")}));
}

TEST(List, Remove)
{
	List a{Id("ginny")};
	EXPECT_EQ(a.Remove(Id("gizmo")), 0);
	EXPECT_EQ(a.size(), 1);
	EXPECT_EQ(a.Remove(Id("ginny")), 1);
	EXPECT_EQ(a.size(), 0);
}

TEST(List, RemoveAssociation)
{
	List a{Association(Id("ginny"), Integer(2))};
	EXPECT_EQ(a.Remove(Id("gizmo")), 0);
	EXPECT_EQ(a.size(), 1);
	EXPECT_EQ(a.Remove(Association(Id("ginny"), Integer(-2))), 0);
	EXPECT_EQ(a.Remove(Id("ginny")), 1);
	EXPECT_EQ(a.size(), 0);
}

TEST(List, Delete)
{
	List a{Id("ginny")};
	List b{Id("gizmo")};
	EXPECT_EQ(a-b, a);
	EXPECT_EQ(a-a, List{});
}

TEST(List, DeleteExactAmount)
{
	List a{Id("ginny"), Id("ginny"), Id("ginny")};
	List b{Id("ginny"), Id("gizmo") ,Id("ginny")};
	EXPECT_EQ(a-b, (List{Id("ginny")}));
}

TEST(List, Union)
{
	List a{Id("ginny"), Id("gizmo")};
	List b{Id("max"), Id("gizmo")};
	EXPECT_EQ(a|b, (List{Id("ginny"), Id("gizmo"), Id("max")}));
}


TEST(List, UnionElementCountIsMax)
{
	List none{};
	List one{Integer(1)};
	List two{Integer(1),Integer(1)};
	List three{Integer(1),Integer(1),Integer(1)};
	EXPECT_EQ(two|one, two);
	EXPECT_EQ(two|three, three);
	EXPECT_EQ(one|none, one);
}

TEST(List, Intersection)
{
	List a{Id("ginny"), Id("gizmo")};
	List b{Id("max"), Id("gizmo")};
	EXPECT_EQ(a&b, (List{Id("gizmo")}));
}

TEST(List, IntersectionElementCountIsMin)
{
	List none{};
	List one{Integer(1)};
	List two{Integer(1),Integer(1)};
	List three{Integer(1),Integer(1),Integer(1)};
	EXPECT_EQ(two&one, one);
	EXPECT_EQ(two&three, two);
	EXPECT_EQ(one&none, none);
}

TEST(List, to_string)
{
	EXPECT_EQ(to_string(List{Id("ginny")}), "[ginny]");
	EXPECT_EQ(to_string(List{Id("ginny"), Id("gizmo")}), "[ginny,gizmo]");
}

}

