#include "pch.h"
#include "Logic/Set.h"
#include "Logic/Knowledge.h"
#include "Logic/Id.h"
#include "Logic/Array.h"
#include "Logic/Boolean.h"

namespace Angel
{
namespace Logic
{
namespace Test
{

TEST(TestSet, Construction)
{
	Set empty;
	EXPECT_EQ(empty.size(), 0);

	Set voidSet{ Object() };
	EXPECT_EQ(voidSet.size(), 0);


	Set single(id(L"ginny"));
	EXPECT_EQ(single.size(), 1);

	Set cats(id(L"ginny"), id(L"max"));
	EXPECT_EQ(cats.size(), 2);

	Set array(array(id(L"ginny"), id(L"max")));
	EXPECT_EQ(array.size(), 2);

	Knowledge k;
	k.Know(set(id(L"table"), id(L"moon"), id(L"hope")));
	EXPECT_EQ(k.Clauses(), 1);
}


TEST(TestSet, Array)
{
	Array array{ set(id(L"cat")), set(id(L"dog")) };
	EXPECT_EQ(2, array.size());
	EXPECT_NE(array, Array(id(L"cat"), id(L"dog")));
}


TEST(TestSet, Compare)
{
	Set a(id(L"ginny"));
	Set b(id(L"ginny"));
	EXPECT_EQ(a, b);

	Set aa(id(L"ginny"), id(L"ginny"));
	EXPECT_EQ(a, aa);

	Set ab(id(L"ginny"), id(L"max"));
	Set ba(id(L"max"), id(L"ginny"));
	EXPECT_EQ(ab, ba);
	EXPECT_NE(a, ab);
	EXPECT_NE(b, ba);

}

}
}
}
