#include <gtest/gtest.h>
#include "Logic/Knowledge.h"
#include "Logic/Boolean.h"
#include "Logic/Integer.h"

namespace Angel::Logic::Test
{

TEST(TestElement, Boolean)
{
	Knowledge k;
	EXPECT_TRUE(k.Query(boolean(true)));
	EXPECT_FALSE(k.Query(boolean(false)));
	EXPECT_EQ(Logic::Boolean(false), Logic::Boolean(false));
	EXPECT_EQ(Logic::Boolean(true), Logic::Boolean(true));
	EXPECT_NE(Logic::Boolean(true), Logic::Integer(2));
}

TEST(TestElement, Integer)
{
	EXPECT_EQ(Logic::Integer(3), Logic::Integer(3));
	EXPECT_NE(Logic::Integer(2), Logic::Integer(-3));
	EXPECT_EQ(Logic::Integer(1), Logic::Boolean(true));

	Knowledge k;
	EXPECT_TRUE(k.Query(integer(1)));
	EXPECT_FALSE(k.Query(integer(0)));
}

}
