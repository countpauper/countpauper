#include "pch.h"
#include "Logic/Knowledge.h"
#include "Logic/Predicate.h"
#include "Logic/Sequence.h"
#include "Logic/Boolean.h"

namespace Angel
{
namespace Logic 
{
namespace Test
{

TEST(TestSolver, Predicate0)
{
	Knowledge k;
	EXPECT_FALSE(k.Query(predicate(L"Test", sequence())));
	k.Know(predicate(L"Test", sequence()));
	EXPECT_TRUE(k.Query(predicate(L"Test", sequence())));
	EXPECT_FALSE(k.Query(predicate(L"Not a Test", sequence())));
}

TEST(TestSolver, Boolean)
{
	Knowledge k;
	EXPECT_TRUE(k.Query(boolean(true)));
	EXPECT_FALSE(k.Query(boolean(false)));
}




}
}
}
