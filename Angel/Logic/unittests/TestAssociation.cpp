#include <gtest/gtest.h>
#include "Logic/Association.h"
#include "Logic/Knowledge.h"
#include "Logic/Expression.h"
#include "Logic/Summation.h"

namespace Angel::Logic::Test
{

TEST(Association, Construction)
{
	Association association{Id("ginny"), Integer(3)};
	EXPECT_TRUE(association);
	EXPECT_EQ(association.size(), 2);
    EXPECT_EQ(association.Left(), Id("ginny"));
    EXPECT_EQ(association.Right(), Integer(3));
    static_assert(sizeof(Association)<=16);
}


TEST(Association, Compare)
{
	Association a{Id("ginny"), Integer(-1)};
	Association b{Id("ginny"), Integer(-1)};
	EXPECT_EQ(a, b);

	Association c{Id("ginny"), Integer(-2)};
	EXPECT_NE(a, c);

	Association d{Id("Ginny"), Integer(-1)};
	EXPECT_NE(a, d);
}


TEST(Association, to_string)
{
	EXPECT_EQ(to_string(Association{Id("ginny"), Integer(4)}), "ginny:4");
	EXPECT_EQ(to_string(Association{Id("ginny"), Id("gizmo")}), "ginny:gizmo");
	EXPECT_EQ(to_string(Association{Predicate("cat"), Id("gizmo")}), "cat()←gizmo");
}

TEST(Association, Simplify)
{
	EXPECT_EQ(Association(Predicate("ginny"), Conjunction{}).Simplify(), Predicate("ginny"));
	EXPECT_EQ(Association(Integer(1), True).Simplify(), Association(Integer(1), True));
}

TEST(Association, SimplifyComparison)
{
	EXPECT_EQ(Association(Variable("X"), Lesser{Integer(0)}).Simplify(), Association(Variable("X"), Lesser{Variable("X"), Integer(0)}));
	EXPECT_EQ(Association(Integer(3), Equal{Integer(3)}).Simplify(), Association(Integer(3), True));
}


TEST(Association, Substitute)
{
	EXPECT_EQ(Association(Predicate("ginny"), Variable("C")).Substitute(Conjunction{Equal{Variable("C"), Id("gizmo")}}),
		Association(Predicate("ginny"), Id("gizmo")));
}

TEST(Association, Matches)
{
	EXPECT_EQ(Association(Id("legs"), Integer(4)).Matches(Id("legs"),{}), Integer(4));
	EXPECT_EQ(Association(Predicate("legs", {Variable("C")}), Integer(4)).Matches(
			Predicate("legs", {Id("ginny")}), {Equal{Variable("C"), Id("ginny")}}), 
			Integer(4));
	EXPECT_EQ(Association(Id("ginny"), Integer(1)).Matches(Association(Id("ginny"), Integer(1)),{}), (Conjunction{True, True}));	
	EXPECT_EQ(Association(Id("gizmo"), Integer(2)).Matches(Association(Id("ginny"), Integer(1)),{}), (Conjunction{False, False}));	
	EXPECT_EQ(Association(Id("ginny"), Integer(3)).Matches(Association(Id("ginny"), Integer(1)),{}), (Conjunction{True, False}));	
}

TEST(Association, Infer)
{
	Knowledge k;
	EXPECT_EQ(k.Infer(Association{Id("five"), Summation({Integer(2), Integer(3)})}), 
        (Association{Id("five"), Integer(5)}));
}


TEST(Association, Get)
{
    EXPECT_EQ((Association{Id("ginny"), Integer(3)}.Get(Id("ginny"))), Integer(3));
    EXPECT_EQ((Association{Id("ginny"), Integer(3)}.Get(Id("gizmo"))), False);
}

}

