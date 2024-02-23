#pragma once
#include "Geometry/Size.h"
#include "Geometry/Mesh.h"
#include "Geometry/Position.h"
#include "UI/Scenery.h"
#include "Game/Material.h"
#include <vector>

namespace Engine
{
    class Image;
};

namespace Game
{
class Map : public Engine::Scenery
{
public:
    explicit Map(Engine::Size size);
    Map(const Engine::Image& data);
    std::string_view Name() const override;
    Engine::Mesh& GetMesh();
    Engine::Coordinate GroundCoord(Engine::Position pos) const;
    Engine::Size GetSize() const;
private:
    struct Grid
    {
        int level = 0;
        const Material* ground = &Material::earth;
        int liquidity = 0;
        const Material* liquid = nullptr;
        const Material* gas = nullptr;
    };

    void GenerateMesh();
    Grid& operator[](Engine::Position pos);
    const Grid& operator[](Engine::Position pos) const;

    static constexpr int subheight = 16;
    Engine::Size size;
    std::vector<Grid> grids;
    Engine::Mesh mesh;
};

}
