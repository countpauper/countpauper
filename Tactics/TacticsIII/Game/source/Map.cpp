#include "Game/Game.h"
#include "Game/Map.h"
#include "File/Image.h"
#include "Geometry/Angles.h"
#include "File/Image.h"
#include "Utility/Maths.h"
#include "Game/Material.h"
#include "UI/Logging.h"
#include <cassert>
#include <span>
#include <cmath>


namespace Game
{

Map::Map(Engine::Size size, std::initializer_list<std::pair<const Material&, unsigned>> map) :
    Scenery(mesh),
    size(size),
    blocks(size.x * size.y * size.z)
{
    auto it = map.begin();
    for(unsigned y=0; y<size.y; ++y)
    {
        for(unsigned x=0;x<size.x; ++x)
        {
            if (it==map.end())
                break;
            if (it->first == Material::water)
                Column(x, y, Material::stone, it->second, it->first, 0);
            else
                Column(x, y, it->first, it->second, Material::air, 0);
            ++it;
        }
    }
    GenerateMesh();
}

const Material* FindMaterial(Engine::HSVA color)
{
    if (color.Value() == 0 )
    {
        return nullptr;
    }
    float nearest = std::numeric_limits<float>::max();
    const Material* result = nullptr;
    float findHue = Engine::Deg2Rad(color.Hue());
    for(auto material: std::span(Material::all))
    {
        Engine::HSVA materialColor(material->color);
        float distance = std::abs(Engine::ShortestTurn(findHue, Engine::Deg2Rad(materialColor.Hue()))) +
            std::abs(color.Saturation() - materialColor.Saturation());
        if (distance < nearest)
        {
            nearest = distance;
            result = material;
        }
    }
    return result;
}

Map::Map(std::string_view filename) :
    Map(filename, Engine::Image(filename))
{
}

Map::Map(std::string_view filename, const Engine::Image& data) :
    Scenery(mesh),
    filename(filename),
    size{int(data.Width()), int(data.Height()/4), (int)LevelToHeight(256)},
    blocks(size.x * size.y * size.z)
{
    for(unsigned y=0; y<size.y; ++y)
    {
        for(unsigned x=0; x< size.x; ++x)
        {
            Engine::RGBA levelPixel = data[Engine::Position(x, y)];
            Engine::HSVA materialPixel(data[Engine::Position(x, y + size.y)]);
            Engine::HSVA liquidPixel(data[Engine::Position(x, y + 2 * size.y)]);
            Engine::HSVA gasPixel(data[Engine::Position(x, y+ 3*size.y)]);

            const auto& liquidMaterial = levelPixel.b > levelPixel.r ? Material::water : Material::air; //FindMaterial(Engine::HSVA(liquidPixel));
            Column(x,y, *FindMaterial(materialPixel), levelPixel.r, liquidMaterial, levelPixel.b);
        }
    }
    GenerateMesh();
}

std::string_view Map::Name() const
{
    return "map";
}

std::string_view Map::FileName() const
{
    return filename;
}

Engine::Mesh& Map::GetMesh()
{
    return mesh;
}

uint32_t Map::Index(Engine::Position pos) const
{
    return pos.x +
        pos.y * size.x +
        pos.z * size.x * size.y;
}

Block& Map::operator[](Engine::Position pos)
{
    return blocks[Index(pos)];
}

const Block& Map::operator[](Engine::Position pos) const
{
    return blocks[Index(pos)];
}


float Map::LevelToHeight(int level) const
{
    return static_cast<float>(level) / subheight;
}
int Map::HeightToLevel(float height) const
{
    return static_cast<int>(height * subheight);
}

float Map::GroundHeight(Engine::Position pos) const
{
    const auto& grid = (*this)[pos];
    for(int z=pos.z; z>=0; --z)
    {
        const auto& block = (*this)[Engine::Position(pos.x, pos.y, z)];
        auto height = block.SolidLevel();
        if (!std::isnan(height))
        {
            return height + z;
        }
    }
    return 0.0f;
}

Engine::IntBox Map::GetBounds() const
{
    return Engine::IntBox(size);
}

void Map::Column(unsigned x, unsigned y, const Material& solid, unsigned solidLvl, const Material& liquid, unsigned liquidLvl)
{
    float solidHeight = LevelToHeight(solidLvl);
    float liquidHeight = LevelToHeight(liquidLvl);

    for(unsigned z=0; z<size.z; ++z)
    {
        auto& block = (*this)[Engine::Position(x,y,z)];

        Engine::Range<float> zRng{float(z), float(z+1)};
        if (zRng[solidHeight])
        {
            if (solid==Material::vegetation)
                block = Block::Vegetation(solidHeight-z - 0.1, 0.1);
            else
                block = Block::Vegetation(0.0f, solidHeight - z);

            if (zRng[liquidHeight] && liquid!=Material::air)
            {
                block.AddWater(liquidHeight - solidHeight);
            }
        }

        else if (zRng < solidHeight)
        {
            block = Block::Stone;
        }
        else if (zRng < liquidHeight)
        {
            assert(liquid == Material::water);  // what does this mean? bad map?
            block = Block::Water;
        }
        else
        {
            block = Block::Air;
        }
    }
}

void Map::GenerateMesh()
{
    int idx = 0;
    Engine::Vector up(0, 0, 1);

    for(unsigned y=0; y<size.y; ++y)
    {
        for(unsigned x=0; x<size.x; ++x)
        {
            for(int z=size.z-1; z>=0; --z)
            {
                const auto& block = (*this)[Engine::Position(x,y,z)];
                const auto& material= block.GetMaterial(Orientation::up);
                if (material==Material::air)
                    continue;
                auto height = block.LiquidLevel();
                if (!std::isnan(height)) {
                    AddQuadToMesh(Engine::Coordinate(x, y, z + height), material);
                }

                height = block.SolidLevel();
                if (!std::isnan(height))
                {
                    if (material == Material::water)    // TODO don't guess, get material profile and pick first solid
                        AddQuadToMesh(Engine::Coordinate(x, y, z + height), Material::stone);
                    else
                        AddQuadToMesh(Engine::Coordinate(x, y, z + height), material);
                    break;
                }
            }
        }
    }
    assert(mesh.names.size() == mesh.triangles.size());
}


void Map::AddQuadToMesh(Engine::Coordinate topleft, const Material& mat)
{
    unsigned vertidx = mesh.vertices.size();
    Engine::Quad quad(
        topleft,
        topleft + Engine::Vector(1,0,0),
        topleft + Engine::Vector(1,1,0),
        topleft + Engine::Vector(0,1,0)
    );
    float brightness = topleft.z / float(size.z);
    brightness = 0.2 + brightness*0.8;
    auto groundColor = mat.color;
    auto vertexColor = Engine::Lerp(Engine::RGBA::black, groundColor, brightness);
    quad.SetColor(vertexColor);
    quad.SetName(Index(Engine::Position(topleft.x, topleft.y, topleft.z)));
    mesh += quad;

    /* TODO sides
    if (topleft.x > 0 )
    {
        Grid& neighbourGrid = grids[idx-1];
        if (neighbourGrid.level != grid.level)
        {
            // TODO add as a quad here too with its own normal and texture coordinates when adding light,
            //  then fake lighting can be removed as that's the complication now to recompute
            unsigned neighbourVertexIdx = vertidx - 4;
            mesh.triangles.push_back({vertidx, vertidx+3, neighbourVertexIdx +1});
            mesh.names.push_back(idx);
            mesh.triangles.push_back({vertidx+3, neighbourVertexIdx +2, neighbourVertexIdx +1});
            mesh.names.push_back(idx);
        }
    }
    if ( topleft.y >0 )
    {
        Grid& neighbourGrid = grids[idx - size.x];
        if (neighbourGrid.level != grid.level)
        {
            unsigned neighbourVertexIdx = vertidx - (4 * size.x);
            mesh.triangles.push_back({vertidx+1, vertidx+0, neighbourVertexIdx +3});
            mesh.names.push_back(idx);
            mesh.triangles.push_back({vertidx+1, neighbourVertexIdx+3, neighbourVertexIdx +2});
            mesh.names.push_back(idx);
        }
    }
        */
}

}
