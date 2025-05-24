#include <gtest/gtest.h>
#include "Logic/Association.h"
#include "Logic/Knowledge.h"
#include "Logic/Expression.h"

namespace Angel::Logic::Test
{

TEST(Association, Construction)
{
	Association association{Id("ginny"), Integer(3)};
	EXPECT_TRUE(association);
	EXPECT_EQ(association.size(), 2);
    EXPECT_EQ(association.Left(), Id("ginny"));
    EXPECT_EQ(association.Right(), Integer(3));
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
}

TEST(Association, Simplify)
{
	EXPECT_EQ(Association(Predicate("ginny"), Conjunction{}).Simplify(), Predicate("ginny"));
	EXPECT_EQ(Association(Integer(1), Boolean(true)).Simplify(), Association(Integer(1), Boolean(true)));
}

TEST(Association, Infer)
{
	Knowledge k;
	EXPECT_EQ(k.Infer(Association{Id("five"), Summation{Integer(2), Integer(3)}}), 
        (Association{Id("five"), Integer(5)}));
}

TEST(Association, Get)
{
    EXPECT_EQ((Association{Id("ginny"), Integer(3)}.Get(Id("ginny"))), Integer(3));
    EXPECT_EQ((Association{Id("ginny"), Integer(3)}.Get(Id("gizmo"))), Boolean(false));
}

}

