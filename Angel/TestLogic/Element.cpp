#include "pch.h"
#include "Logic/Knowledge.h"
#include "Logic/Boolean.h"
#include "Logic/Integer.h"

namespace Angel
{
namespace Logic 
{
namespace Test
{

TEST(TestElement, Boolean)
{
	Knowledge k;
	EXPECT_TRUE(k.Query(boolean(true)));
	EXPECT_FALSE(k.Query(boolean(false)));
	EXPECT_EQ(Logic::Boolean(true), Logic::Integer(2));
	EXPECT_EQ(Logic::Boolean(false), Logic::Integer(0));
	EXPECT_NE(Logic::Boolean(false), Logic::Integer(-3));
}

TEST(TestElement, Integer)
{
	EXPECT_EQ(Logic::Integer(3), Logic::Integer(3));
	EXPECT_NE(Logic::Integer(2), Logic::Integer(-3));
	EXPECT_EQ(Logic::Integer(2), Logic::Boolean(true));
	EXPECT_EQ(Logic::Integer(0), Logic::Boolean(false));
	EXPECT_NE(Logic::Integer(2), Logic::Boolean(false));


	Knowledge k;
	EXPECT_TRUE(k.Query(integer(1)));
	EXPECT_FALSE(k.Query(integer(0)));
}



}
}
}
