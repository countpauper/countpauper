#pragma once
#include "Geometry/Size.h"
#include "Geometry/Mesh.h"
#include "Geometry/Position.h"
#include "UI/Scenery.h"
#include "Game/HeightMap.h"
#include "Game/Material.h"
#include "Game/Block.h"
#include <vector>

namespace Engine
{
    class Image;
};

namespace Game
{

class Map :
    public Engine::Scenery,
    public HeightMap
{
public:
    explicit Map(Engine::Size size, std::initializer_list<std::pair<const Material&, int>> map={});
    explicit Map(std::string_view fileName);
    std::string_view Name() const override;
    std::string_view FileName() const;
    Engine::Mesh& GetMesh();
    // Height Map
    Engine::IntBox GetBounds() const override;
    Engine::Coordinate GroundCoord(Engine::Position pos) const override;
    float GroundHeight(Engine::Position pos) const override;

private:
    Map(std::string_view fileName, const class Engine::Image& data);
    unsigned Index(Engine::Position pos) const;
    void Column(unsigned x, unsigned y, const Material& solid, unsigned solidLvl, const Material& liquid, unsigned liquidLvl);
    void AddQuadToMesh(Engine::Coordinate topleft, const Material& mat);

    void GenerateMesh();
    Block& operator[](Engine::Position pos);
    const Block& operator[](Engine::Position pos) const;

    float LevelToHeight(int level) const;
    int HeightToLevel(float height) const;

    static constexpr int subheight = 16;
    std::string filename;
    Engine::Size size;
    std::vector<Block> blocks;
    Engine::Mesh mesh;
};

}
