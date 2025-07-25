#include <gtest/gtest.h>
#include "Logic/Knowledge.h"
#include "Logic/Id.h"
#include "Logic/Expression.h"

namespace Angel::Logic::Test
{

TEST(Id, Construction)
{
	EXPECT_FALSE(Id(""));
	EXPECT_TRUE(Id("test"));
	EXPECT_EQ(Logic::Id("test"), Logic::Id("test"));
	EXPECT_NE(Logic::Id("test"), Logic::Id("icle"));
	static_assert(sizeof(Id)<=32);
}

TEST(Id, Infer)
{
	Knowledge k;
	EXPECT_EQ(k.Infer(Id("test")), Id("test"));
	EXPECT_EQ(k.Infer(Id("")), Id(""));
}

}
