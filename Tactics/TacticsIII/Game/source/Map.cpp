#include "Game/Game.h"
#include "Game/Map.h"

#include "Game/Material.h"
#include "File/Image.h"
#include "Geometry/Angles.h"
#include "File/Image.h"
#include "Utility/Maths.h"
#include "UI/Logging.h"
#include "Utility/Assert.h"
#include <span>
#include <cmath>


namespace Game
{

Map::Map(Engine::Size size, std::initializer_list<std::pair<const Material&, ZType>> map) :
    Scenery(mesh),
    size(size),
    slices(size.x * size.y)
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
    slices(size.x * size.y)
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
            ZType solidHeight = LevelToHeight(levelPixel.r);
            ZType liquidHeight = LevelToHeight(levelPixel.b);
            const Material* mat = FindMaterial(materialPixel); 
            assert(mat);
            Column(x,y, *mat , solidHeight, liquidMaterial, liquidHeight);
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


ZType Map::LevelToHeight(int level) const
{
    return ZType(level) / subheight;
}

int Map::HeightToLevel(ZType height) const
{
    return static_cast<int>(height * subheight);
}


Engine::IntBox Map::GetBounds() const
{
    return Engine::IntBox(size);
}

unsigned Map::SliceIdx(int x, int y) const
{
    return y*size.x + x;
}

const Slice& Map::SliceAt(int x, int y) const
{
    return slices.at(SliceIdx(x, y));
}

Position Map::IdToPosition(uint32_t id) const
{
    auto bounds = GetBounds();
    return Position{
        static_cast<int>((id                                      ) % bounds.x.Size()),
        static_cast<int>((id /  bounds.x.Size()                   ) % bounds.y.Size()),
        static_cast<int>((id / (bounds.x.Size() * bounds.y.Size())) % bounds.z.Size())
    } + Position(bounds.Start());
}


void Map::Column(unsigned x, unsigned y, const Material& solid, ZType solidLvl, const Material& liquid, ZType liquidLvl, float temperature)
{
    auto mapHeight  = Z();
    auto& slice = slices.at(SliceIdx(x,y));
    static const ZType maxVeg(0.1);
    static const ZType maxEarth(2.0);
    ZType stoneLevel=0.0;
    ZType earthLevel=0.0;
    ZType vegLevel=0.0;
    if (solidLvl >= mapHeight.begin)
    {
        solidLvl -= mapHeight.begin;
        if (solid == Material::vegetation)
        {
            stoneLevel = solidLvl > maxVeg + maxEarth ? solidLvl - maxVeg - maxEarth : 0.0;
            earthLevel = (solidLvl > maxVeg ? std::max(solidLvl - maxVeg, maxEarth)  : 0.0f) - stoneLevel;
            vegLevel = std::min( solidLvl, maxVeg ); 
        } 
        else if (solid == Material::earth)
        {
            stoneLevel = solidLvl > maxEarth ? solidLvl - maxEarth : 0.0;
            earthLevel = std::min(solidLvl, maxEarth);
        }
        else if (solid == Material::stone)
        {
            stoneLevel = solidLvl;
        } 
        if (stoneLevel)
            slice.emplace_back(Material::stone, stoneLevel, temperature);
        if (earthLevel)
            slice.emplace_back(Material::earth, earthLevel, temperature);
        if (vegLevel)
            slice.emplace_back(solid, vegLevel, temperature);
    }
    if (liquidLvl > solidLvl)
    {
        slice.emplace_back(liquid, liquidLvl - solidLvl, temperature);
        slice.emplace_back(Material::air, mapHeight.end - liquidLvl, temperature);
    }
    else 
    {
        slice.emplace_back(Material::air, mapHeight.end - solidLvl, temperature);
    }
}

uint8_t ComputeOpacityAtDepth(uint8_t alpha, ZType depth)
{
    static constexpr float maxAlpha = 255.0f;
    float o1 = alpha / maxAlpha;    // Alpha channel of material is defined as opacity at 1 meter
    float m = float(depth);
    float o = o1 * std::pow(m, 1.0f - o1);
    return static_cast<Engine::RGBA::Component>(o * maxAlpha);
}


ZType& Map::SurroundingHeights::operator[](Orientation ori)
{
    auto idx = ori.Index();
    if (idx<0)
        throw std::range_error("Invalid surrounding height");
    else 
        return height[ori.Index()];
}

ZType Map::SurroundingHeights::operator[](Orientation ori) const
{
    auto idx = ori.Index();
    if (idx<0)
        return ZType(0.0);
    else
        return height[ori.Index()];
}


void Map::GenerateMesh()
{
    int idx = 0;
    Engine::Vector up(0, 0, 1);

    for(unsigned y=0; y<size.y; ++y)
    {
        for(unsigned x=0; x<size.x; ++x)
        {
            const auto& slice = SliceAt(x, y);
            ZType height = 0.0;
            for(auto layer : slice) 
            {
                height += layer.amount;
                const auto& material= layer.material;
                auto color = material.get().color;
                // todo: should compute each vertex as average of cornered squares for smooth interpolation 
                color.a = ComputeOpacityAtDepth(color.a, layer.amount); 
                SurroundingHeights heights = CalculateSurroundingHeights({static_cast<int>(x), static_cast<int>(y), height}, slice);
                AddLayerToMesh(Position(x, y, height), color, heights);                
            }
        }
    }
    assert(mesh.Names().size() == mesh.Triangles().size());
}

ZType SumAmount(Slice::const_iterator from, Slice::const_iterator to)
{
    return std::accumulate(from, to, ZType(0.0),[](ZType sum, const Layer& layer)
    {
        return sum + layer.amount;
    });
}

Map::SurroundingHeights Map::CalculateSurroundingHeights(Position p, const Slice& centerSlice)
{
    SurroundingHeights result;
    auto [it, amount] = centerSlice.Find(p.Z());
    const auto& layer = *it;
    if (it==centerSlice.end())
        result[Orientation::up] = 0;
    else if (it->IsTranslucent())
        result[Orientation::up] = p.Z() - amount;
    else
    {
        result[Orientation::up] = p.Z() - amount + layer.amount + 
            SumAmount(it+1, std::find_if_not(it+1, centerSlice.end(), std::mem_fn(&Layer::IsOpaque)));
        assert(result[Orientation::up]>=p.Z());
    }
    result[Orientation::down] = amount; // TODO if there's a previous one and it's gas, add a bottom ? 
    result[Orientation::left] = 0.0;
    result[Orientation::right] = 0.0;
    result[Orientation::front] = 0.0;
    result[Orientation::back] = 0.0;
    
    // TODO: first do this slice vertical for up, none and down orientations 
    // Then the horizontal ones and use the same helpers as can be used for move:
    //     Slice::FindSolidDownFrom(height) and FindNonSolidUpFrom(height) or FindDownFromHeight(predicate) 
    //    Might need to iterator Slice::At(height) 
    //   Add unit tests to all these slices functions at leasts, because these mesh generation functions/helpers are harder to test (and maintain since it's all not as required)

    return result;
}

void Map::AddLayerToMesh(Position pos, Engine::RGBA vertexColor, const SurroundingHeights& heights)
{
    // TODO if neighbourheight +Z > topLeft.Z(), then skip it
    // if neighbourheid -Z then that's the maximum side height 
    // height at none is mine in ztype (for accurate compare) so can pass x and y as integer position, for name or just redundant 
    // else for sided: if they are higher or equal then skip, of lower then have vertical quads on that side up to the maximum of that height and the -Z one 

    Engine::Coordinate topleft(
        static_cast<double>(pos.X()),
        static_cast<double>(pos.Y()),
        static_cast<double>(pos.Z())
    );
    Engine::Quad quad(
        topleft,
        topleft + Engine::Vector(1,0,0),
        topleft + Engine::Vector(1,1,0),
        topleft + Engine::Vector(0,1,0)
    );
    if (!vertexColor)
        return;
    quad.SetColor(vertexColor);
    quad.SetName(Index(Engine::Position(topleft.X(), topleft.Y(), topleft.Z())));
    if (heights[Orientation::up] <= pos.Z())    // not occluded by layer above
    {
        mesh += quad;
    }

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
