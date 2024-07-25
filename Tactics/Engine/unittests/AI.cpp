#include <gtest/gtest.h>
#include "AI/Astar.h"

namespace Engine::Test
{
    struct Position
{
    int x,y;
    bool operator==(const Position& rhs) const
    {
        return x==rhs.x && y==rhs.y;
    }
};
}
namespace std
{
template <> class hash<Engine::Test::Position>
{
public:
    size_t operator()(const Engine::Test::Position &p) const
    {
        return p.x + p.y*65536;
    }
};
}
namespace Engine::Test
{

static uint8_t map[] {
    0b00010000,
    0b01110110,
    0b00010000,
    0b11011101,
    0b11000001,
};
auto manhattan = [](Position a, Position b) -> int
{
    // this is admissable (doesnt over estimate) even with 8 way movement
    // because diagonals also cost manhattan distance (2)
    return std::abs(a.x - b.x) + std::abs(a.y - b.y);
};
auto neighbours = [](Position p) -> std::vector<Position>
{
    std::vector<Position> result;
    for(int y=-1; y<=1;y++)
    {
        if (p.y + y<0)
            continue;
        if (p.y + y > int(sizeof(map)))
            continue;
        for(int x=-1; x<=1;x++)
        {
            if (x==0 && y == 0)
                continue;
            Position np {p.x + x, p.y + y};
            if (np.x < 0 )
                continue;
            if (np.x >= int(sizeof(map[0])*8))
                continue;
            if (map[np.y] & (1 << np.x))
                continue;
            result.push_back(np);
        }
    }
    return result;
};

TEST(AI, EasyAstar)
{
    std::vector<Position> exepectedPath{
        {0,0},
        {1,0},
        {2,0},
        {3,1},
        {3,2}
    };
    EXPECT_EQ((Astar::Plan<Position, int>({0,0}, {3,2}, manhattan, neighbours)), exepectedPath);
}

TEST(AI, HardAstar)
{
    std::vector<Position> exepectedPath{
        {5,0},
        {6,0},
        {7,1},
        {6,2},
        {5,3},
        {4,4},
        {3,4},
        {2,4},
        {1,3},
        {2,2},
        {3,1},
        {3,0}
    };
    EXPECT_EQ((Astar::Plan<Position, int>({5,0}, {3,0}, manhattan, neighbours)), exepectedPath);
}

TEST(AI, MultipleDestinations)
{
    std::vector<Position> exepectedPath{
        {0,0},
        {0,1},
        {1,2},
        {2,2}
    };
    Position destinations[] {{3,2},{2,2}};
    EXPECT_EQ((Astar::Plan<Position, int>({0,0}, destinations, manhattan, neighbours)), exepectedPath);
}
}
