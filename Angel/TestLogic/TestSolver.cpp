#include "pch.h"
#include "Logic/Knowledge.h"
#include "Logic/Predicate.h"

namespace Angel
{
namespace Logic 
{
namespace Test
{

TEST(TestSolver, Knowledge)
{
	Knowledge k;

	EXPECT_FALSE(k.Query(Create<Predicate>(L"Test")));
	k.Know(Create<Predicate>(L"Test"));
	EXPECT_TRUE(k.Query(Create<Predicate>(L"Test")));
	EXPECT_FALSE(k.Query(Create<Predicate>(L"Not a Test")));
}

}
}
}
