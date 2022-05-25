#include "pch.h"
#include "Parser/Parser.h"
#include "Logic/Predicate.h"
#include "Logic/Sequence.h"
#include "Logic/Boolean.h"
#include "Logic/Integer.h"

namespace Angel
{
namespace Parser
{
namespace Test
{

    /* Reenable with BNF

TEST(TestElement, Id)
{
    std::wstringstream s(L"cat");
    Logic::Object cat;
    s >> cat;
    EXPECT_EQ(cat, Logic::id(L"cat"));
}

TEST(TestElement, Ids)
{
    std::wstringstream s(L"gizmo ginny");
    Logic::Object gizmo, ginny;
    s >> gizmo >> ginny;
    EXPECT_EQ(gizmo, Logic::id(L"gizmo"));
	EXPECT_EQ(ginny, Logic::id(L"ginny"));
}

TEST(TestElement, Boolean)
{
    std::wstringstream s(L"true false");
    Logic::Object yes, no;
    s >> yes >> no;

    EXPECT_EQ(yes, Logic::boolean(true));
    EXPECT_EQ(no, Logic::boolean(false));
}

TEST(TestElement, Integer)
{
    std::wstringstream s(L"-9 23 cat2");
    Logic::Object negative, positive, id;
    s >> negative >> positive >> id;

    EXPECT_EQ(positive, Logic::integer(23));
    EXPECT_EQ(negative, Logic::integer(-9));
    EXPECT_EQ(id, Logic::id(L"cat2"));
}
*/



}
}
}

