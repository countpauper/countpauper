#pragma once
#include "Geometry/Size.h"
#include "Geometry/Mesh.h"
#include "Geometry/Position.h"
#include "UI/Scenery.h"
#include "Game/HeightMap.h"
#include "Game/Material.h"
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
    explicit Map(Engine::Size size);
    explicit Map(std::string_view fileName);
    std::string_view Name() const override;
    std::string_view FileName() const;
    Engine::Mesh& GetMesh();
    // Height Map
    Engine::Size GetSize() const override;
    Engine::Coordinate GroundCoord(Engine::Position pos) const override;
    int GroundHeight(Engine::Position pos) const override;
    const Material& GetMaterial(Engine::Position pos) const override;
private:
    struct Grid
    {
        int level = 0;
        const Material* ground = &Material::earth;
        int liquidity = 0;
        const Material* liquid = nullptr;
        const Material* gas = nullptr;
    };
    Map(std::string_view fileName, const class Engine::Image& data);

    void GenerateMesh();
    Grid& operator[](Engine::Position pos);
    const Grid& operator[](Engine::Position pos) const;

    static constexpr int subheight = 16;
    std::string filename;
    Engine::Size size;
    std::vector<Grid> grids;
    Engine::Mesh mesh;
};

}
