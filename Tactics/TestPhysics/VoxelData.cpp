#include "pch.h"
#include "Physics/VoxelData.h"

namespace Physics::Test
{

TEST(VoxelData, Construction)
{
    VoxelData data(Size(1, 2, 3));
    EXPECT_EQ(Size(1, 2, 3), data.GetSize());
}

TEST(VoxelData, Indexing)
{
    VoxelData data(Size(2));
    EXPECT_EQ(&Material::vacuum, data[Position(1, 0, 1)].material);
    EXPECT_EQ(0, data[Position(1, 1, 1)].density); // inside
    EXPECT_THROW(data[Position(-1, 0, 0)].density, std::out_of_range); // edge
    EXPECT_THROW(data[Position(-1, 2, 0)].density, std::out_of_range);  // corner
}


TEST(VoxelData, IndexingWithEdge)
{
    VoxelData data(Size(2), 1);
    EXPECT_EQ(&Material::vacuum, data[Position(1, 0, 1)].material);
    EXPECT_EQ(0, data[Position(1, 1, 1)].density); // inside
    EXPECT_EQ(0, data[Position(-1, 0, 0)].density); // edge
    EXPECT_EQ(0, data[Position(-1, 2, 0)].density);  // corner
    EXPECT_THROW(data[Position(3, 0, 1)], std::out_of_range);
    EXPECT_THROW(data[Position(1, -2, 0)], std::out_of_range);
}

TEST(VoxelData, AllWithEdge)
{
    VoxelData data(Size(3, 2, 1), 1);
    auto all = data.All();
    EXPECT_EQ(Position(5, 4, 3), all.end() - all.begin());
}

TEST(VoxelData, IsInside)
{
    VoxelData data(Size(3, 2, 1));
    EXPECT_TRUE(data.IsInside(Position(0, 0, 0)));
    EXPECT_TRUE(data.IsInside(Position(2, 1, 0)));
    EXPECT_FALSE(data.IsInside(Position(0, -1, 0)));
    EXPECT_FALSE(data.IsInside(Position(1, 1, 1)));

    EXPECT_TRUE(std::all_of(data.begin(), data.end(), [&data](const decltype(data)::value_type& v)
    {
        return data.IsInside(v.first);
    }));
}


TEST(VoxelData, AABBSection)
{
    VoxelData data(Size(3, 2, 1));
    Engine::AABB bounds(Engine::Range<double>(1, 3), Engine::Range<double>(-5, 5), Engine::Range<double>(0, 1));
    auto section = data.In(bounds);
    EXPECT_TRUE(std::all_of(section.begin(), section.end(), [&bounds, data](const decltype(data)::value_type& v)
    {
        return bounds.Contains(data.Center(v.first));
    }));
}

TEST(VoxelData, Section)
{
    VoxelData data(Size(3, 2, 1));
    Box bounds(Position(1, -5, 0), Position(3, 5, 1));
    auto section = data.In(bounds);
    EXPECT_TRUE(std::all_of(section.begin(), section.end(), [&bounds](const decltype(data)::value_type& v)
    {
        return bounds.Contains(v.first);
    }));
}

TEST(VoxelData, NoneBoundary)
{
    VoxelData data(Size(3, 2, 1), 1);
    auto middle = data.BoundaryCondition(Direction::none);
    for (const auto& v : middle)
    {
        EXPECT_TRUE(data.IsInside(v.first));
        EXPECT_TRUE(data.IsBoundary(v.first).empty());
    }
}

TEST(VoxelData, Boundary)
{
    VoxelData data(Size(3, 2, 1), 1);
    for (auto dir : Direction::all)
    {
        auto boundary = data.BoundaryCondition(dir);
        for (const auto& v : boundary)
        {
            EXPECT_FALSE(data.IsInside(v.first)) << " for " << v.first << " in " << dir;
            EXPECT_TRUE(data.IsBoundary(v.first)[dir]) << " for " << v.first << " in " << dir;
        }
    }
}

TEST(VoxelData, Iterator)
{
    VoxelData data(Size(2));
    unsigned count = 0;
    for (const auto& v : data)
        ++count;
    EXPECT_EQ(8, count);
    EXPECT_EQ(Position(2, 2, 2), data.end() - data.begin());
    auto first = *data.begin();
    EXPECT_EQ(Position(0, 0, 0), first.first);
    EXPECT_FALSE(data.IsBoundary(first.first));
}

TEST(VoxelData, BoundaryGrids)
{
    VoxelData data(Size(2));
    EXPECT_EQ(Direction::east | Direction::north | Direction::up, data.IsBoundary(Position(2, 2, 2)));
    EXPECT_EQ(Direction::west | Direction::south | Direction::down, data.IsBoundary(Position(-1, -1, -1)));
}

}
