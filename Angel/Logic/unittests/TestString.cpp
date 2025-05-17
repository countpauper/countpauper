#include <gtest/gtest.h>
#include "Logic/Knowledge.h"
#include "Logic/String.h"

namespace Angel::Logic::Test
{

TEST(String, Construction)
{
	EXPECT_FALSE(String(""));
	EXPECT_TRUE(String(" "));
	EXPECT_EQ(String("test"), String("test"));
	EXPECT_NE(String("test"), String("Test"));
}

TEST(String, Cast)
{
	EXPECT_EQ(String(Boolean(true)), String("true"));
	EXPECT_EQ(String(Integer(-3)), String("-3"));
}

TEST(String, Infer)
{
	Knowledge k;
	EXPECT_EQ(k.Infer(String("test")), String("test"));
}


}
