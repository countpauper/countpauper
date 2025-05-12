#include <gtest/gtest.h>
#include "Logic/List.h"
#include "Logic/Knowledge.h"
#include "Logic/Id.h"
#include "Logic/Boolean.h"

namespace Angel::Logic::Test
{

TEST(TestList, Construction)
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
   
    Knowledge k;
	EXPECT_THROW(k.Know(List{Id("table"), Id("moon"), Id("hope")}), std::invalid_argument);
}


TEST(TestList, Compare)
{

	List a{Id("ginny")};
	List b{Id("ginny")};
	EXPECT_EQ(a, b);

	List ab{Id("ginny"), Id("max")};
	List ba{Id("max"), Id("ginny")};
	EXPECT_NE(ab, ba);
}

}

