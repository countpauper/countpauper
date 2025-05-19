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

	static_assert(Logic::IsCollection<List>);
}


TEST(List, Compare)
{
	List a{Id("ginny")};
	List b{Id("ginny")};
	EXPECT_EQ(a, b);

	List ab{Id("ginny"), Id("max")};
	List ba{Id("max"), Id("ginny")};
	EXPECT_NE(ab, ba);
}


TEST(List, to_string)
{
	EXPECT_EQ(to_string(List{Id("ginny")}), "[ginny]");
	EXPECT_EQ(to_string(List{Id("ginny"), Id("gizmo")}), "[ginny,gizmo]");
}

TEST(List, Infer)
{
	Knowledge k;
	EXPECT_EQ(k.Infer(List{Summation{Integer(2), Integer(3)}}), (List{Integer(5)}));
}

TEST(List, Get)
{
	EXPECT_EQ(List().Get(Id("ginny")), Integer(0));
	EXPECT_EQ((List{Id("ginny"), Id("ginny")}).Get(Id("ginny")), Integer(2));
	EXPECT_EQ((List{
		Association{Id("ginny"), Integer(1)},
		Association{Id("gizmo"), Integer(2)},
		Association{Id("ginny"), Integer(3)},
		Id("ginny")}).Get(Id("ginny")), (List{Integer(1), Integer(3), Boolean(true)}));
}

}

