#pragma once
#include "Game/MapItf.h"
#include "Game/Stack.h"
#include "Game/Material.h"
#include "Game/Orientation.h"
#include "Geometry/Size.h"
#include "Geometry/Mesh.h"
#include "Geometry/Position.h"
#include "UI/Scenery.h"
#include <vector>

namespace Engine
{
    class Image;
};

namespace Game
{

class Map :
    public Engine::Scenery,
    public MapItf
{
public:
    explicit Map(Engine::Size size, std::initializer_list<std::pair<const Material&, ZType>> map={});
    explicit Map(std::string_view fileName);
    std::string_view Name() const override;
    std::string_view FileName() const;
    Engine::Mesh& GetMesh();
    Position NameToPosition(uint32_t name) const;

    Stack::iterator Fill(Position at, ZType height, const Material& mat, Cell::Temperature temperature=300.0);
    // Map Itf
    Engine::IntBox GetBounds() const override;
    const Stack& StackAt(int x, int y) const override; 
    Stack& StackAt(int x, int y);
    inline Stack& operator[](int x, int y) { return StackAt(x,y); };
private:
    Map(std::string_view fileName, const class Engine::Image& data);
    unsigned Name(Position pos) const;
    unsigned StackIdx(int x, int y) const;
    void Column(unsigned x, unsigned y, const Material& solid, ZType solidLvl, const Material& liquid, ZType liquidLvl, float temperature=300.0f);
 
    // TODO Split off to a map mesh generator helper 
    struct NeighbourHeights 
    {
        ZType& operator[](Orientation ori);
        ZType operator[](Orientation ori) const;
    private:
        ZType height[7];
    };
    NeighbourHeights CalculateNeighbourHeights(Position p, const Stack& centerSlice);
    void AddCellToMesh(Position pos, Engine::RGBA vertexColor, const NeighbourHeights& neighbourHeight);
    void GenerateMesh();


    std::string filename;
    Engine::Size size;
    std::vector<Stack> stacks;
    Engine::Mesh mesh;
};

}
