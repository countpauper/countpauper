#include "pch.h"
#include "Game/Location.h"

namespace Game
{
namespace Test
{

TEST(LocationTest, Null)
{
    Location a;
    EXPECT_FALSE(a);
}

TEST(LocationTest, ContainsPlane)
{
    Location a(Physics::Plane::all, 0);
    EXPECT_TRUE(a.Contains(Location(Physics::Plane::front, 0)));
    EXPECT_TRUE(a.Contains(Location(Physics::Plane::back, 0)));
    EXPECT_TRUE(a.Contains(Location(Physics::Plane::left, 0)));
    EXPECT_TRUE(a.Contains(Location(Physics::Plane::right, 0)));
	EXPECT_TRUE(a.Contains(Location(Physics::Plane::top, 0)));
	EXPECT_TRUE(a.Contains(Location(Physics::Plane::bottom, 0)));
	EXPECT_TRUE(a.Contains(Location(Physics::Plane::sagittal, 0)));
	EXPECT_TRUE(a.Contains(Location(Physics::Plane::coronal, 0)));
	EXPECT_TRUE(a.Contains(Location(Physics::Plane::transversal, 0)));
	Location b(Physics::Plane::sagittal, 1);
	EXPECT_TRUE(b.Contains(Location(Physics::Plane::front, 1)));
	EXPECT_TRUE(b.Contains(Location(Physics::Plane::back, 1)));
	EXPECT_TRUE(b.Contains(Location(Physics::Plane::around, 1)));
	EXPECT_FALSE(b.Contains(Location(Physics::Plane::left, 1)));
	EXPECT_FALSE(b.Contains(Location(Physics::Plane::right, 1)));
	EXPECT_FALSE(b.Contains(Location(Physics::Plane::bottom, 1)));
	EXPECT_FALSE(b.Contains(Location(Physics::Plane::top, 1)));
	EXPECT_FALSE(b.Contains(Location(Physics::Plane::coronal, 1)));
	EXPECT_FALSE(b.Contains(Location(Physics::Plane::transversal, 1)));
}


TEST(LocationTest, ContainsHeight)
{
    Location a(Physics::Plane::front, 1, 2);
    EXPECT_FALSE(a.Contains(Location(Physics::Plane::front, 0)));
    EXPECT_TRUE(a.Contains(Location(Physics::Plane::front, 1)));
    EXPECT_TRUE(a.Contains(Location(Physics::Plane::front, 2)));
    EXPECT_FALSE(a.Contains(Location(Physics::Plane::front, 3)));
    EXPECT_TRUE(a.Contains(Location(Physics::Plane::front, 0, 2)));
    EXPECT_FALSE(a.Contains(Location(Physics::Plane::front, 3, 2)));
}

TEST(LocationTest, Serialize)
{
    std::wstringstream stream(L"Front[1] Around[2-3]");
    Location a, b;
    stream >> a >> b;
    EXPECT_EQ(a.plane, Physics::Plane::front);
    EXPECT_EQ(a.position, 1);
    EXPECT_EQ(a.size, 1);
    EXPECT_EQ(b.plane, Physics::Plane::around);
    EXPECT_EQ(b.position, 2);
    EXPECT_EQ(b.size, 2);
}

TEST(LocationTest, SerializeFail)
{
    std::wstringstream stream(L"Front Back][ Left[ Right] None[]");
    Location f, b, l, r, n;
    EXPECT_THROW(stream >> f, std::exception);
    EXPECT_THROW(stream >> b, std::exception);
    EXPECT_THROW(stream >> l, std::exception);
    EXPECT_THROW(stream >> r, std::exception);
    EXPECT_THROW(stream >> n, std::exception);
}

}
}