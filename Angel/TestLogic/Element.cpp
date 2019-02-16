#include "pch.h"
#include "Logic/Knowledge.h"
#include "Logic/Predicate.h"
#include "Logic/Sequence.h"
#include "Logic/Boolean.h"
#include "Logic/Clause.h"

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
}

}
}
}
