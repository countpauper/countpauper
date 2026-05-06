#pragma once
#include "Game/MapItf.h"
#include "Game/Slice.h"
#include "Game/Material.h"

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
    Position IdToPosition(uint32_t id) const;
    // Map Itf
    Engine::IntBox GetBounds() const override;
    const Slice& SliceAt(int x, int y) const override; 
private:
    Map(std::string_view fileName, const class Engine::Image& data);
    unsigned Index(Engine::Position pos) const;
    void Column(unsigned x, unsigned y, const Material& solid, ZType solidLvl, const Material& liquid, ZType liquidLvl, float temperature=300.0f);
    void AddQuadToMesh(Engine::Coordinate topleft, const Material& mat);

    unsigned SliceIdx(int x, int y) const;

    void GenerateMesh();

    ZType LevelToHeight(int level) const;
    int HeightToLevel(ZType height) const;

    static constexpr int subheight = 16;
    std::string filename;
    Engine::Size size;
    std::vector<Slice> slices;
    Engine::Mesh mesh;
};

}
