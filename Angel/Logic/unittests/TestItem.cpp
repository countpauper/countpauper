#include <gtest/gtest.h>
#include "Logic/Item.h"
#include "Logic/Knowledge.h"
#include "Logic/Expression.h"
#include "Logic/Summation.h"

namespace Angel::Logic::Test
{

TEST(Item, Construction)
{
	Item item{List{}, Integer(3)};
	EXPECT_TRUE(item);
    EXPECT_EQ(item.Left(), List());
    EXPECT_EQ(item.Right(), Integer(3));
    static_assert(sizeof(Item)<=16);
}


TEST(Item, Compare)
{
	Item a{List{Id("ginny")}, Id("ginny")};
	Item b{List{Id("ginny")}, Id("ginny")};
	EXPECT_EQ(a, b);

	Item c{List{Id("ginny")}, Id("gizmo")};
	EXPECT_NE(a, c);

	Item d{List{Id("max")}, Id("ginny")};
	EXPECT_NE(a, d);
}


TEST(Item, to_string)
{
	EXPECT_EQ(to_string(Item{Variable("ginny"), Id("gizmo")}), "$ginny.gizmo");
	EXPECT_EQ(to_string(Item{List{Id("ginny")}, Id("gizmo")}), "[ginny].gizmo");
}

TEST(Item, Simplify)
{
	EXPECT_EQ(Item(Set{Id("ginny")}, Id("gizmo")).Simplify(), False);
	EXPECT_EQ(Item(List{Id("ginny")}, Id("ginny")).Simplify(), Integer(1));
	EXPECT_EQ(Item(Variable("ginny"), Id("ginny")).Simplify(), Item(Variable("ginny"), Id("ginny")));
	EXPECT_EQ(Item(Set{Association(Id("ginny"),Id("cat"))},Id("ginny")).Simplify(), Id("cat"));
}


TEST(Item, Substitute)
{
	EXPECT_EQ(Item(Variable("C"), Id("ginny")).Substitute(Conjunction{Equal{Variable("C"), List{Id("cat")}}}),
		Item(List{Id("cat")}, Id("ginny")));
}

TEST(Item, Matches)
{

    // $V.x, $V=[x:3] matches 3  
    EXPECT_EQ(Item(Variable("V"),Id("x")).Matches(Integer(3), 
                                {Equal{Variable("V"), Set{Association(Id("x"), Integer(3))}}}).Simplify(), 
                                True);
    // TODO [x:3].$X, matches 3 if x=$X
    // TODO $V.x, matches 3 if x:3@$V
    // TODO [x:$V].x matchex 3 if $V=3
    // TODO [$X:3].x matches 3 if $X=x
    // All this is done by matching rhs:expression with all elements in left 
}

TEST(Item, Infer)
{
	Knowledge k;
	EXPECT_EQ(k.Infer(Item{Id("five"), Summation({Integer(2), Integer(3)})}), 
        (Item{Id("five"), Integer(5)}));
}


}

