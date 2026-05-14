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
#include <ranges>


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
            {
                Column(x,y, Material::stone, 0, Material::air, size.z);
                continue;
            }
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

uint32_t Map::Index(Position pos) const
{
    return pos.x +
        pos.y * size.x +
        pos.z.RawValue() * size.x * size.y;
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
        ZType::FromRaw(static_cast<int>(id / (bounds.x.Size() * bounds.y.Size())))
    } + Position(bounds.Start());
}


Slice::iterator Map::Fill(Position at, ZType height, const Material& mat, Layer::Temperature temperature)
{
    auto& slice = slices.at(SliceIdx(at.X(), at.Y()));
    return slice.Fill(Engine::Range<ZType>(at.Z(), at.Z()+height), mat, temperature);
}


void Map::Column(unsigned x, unsigned y, const Material& solid, ZType solidLvl, const Material& liquid, ZType liquidLvl, float temperature)
{
    auto mapHeight  = Z();
    auto& slice = slices.at(SliceIdx(x,y));
    static const ZType maxVeg(0.25);
    static const ZType maxEarth(2.0);
    ZType stoneLevel=0.0;
    ZType earthLevel=0.0;
    ZType vegLevel=0.0;
    ZType airLvl=0.0;
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
        airLvl = solidLvl;
    }

    if (liquidLvl > solidLvl)
    {
        slice.emplace_back(liquid, liquidLvl - solidLvl, temperature);
        airLvl = liquidLvl;
    }
    
    if (airLvl<mapHeight.end)
    {
        slice.emplace_back(Material::air, mapHeight.end - airLvl, temperature);
    }
}

uint8_t ComputeOpacityAtDepth(uint8_t alpha, ZType depth)
{
     // todo: should compute each vertex as average of cornered squares for smooth interpolation 

    static constexpr float maxAlpha = 255.0f;
    float o1 = alpha / maxAlpha;    // Alpha channel of material is defined as opacity at 1 meter
    float m = float(depth);
    float o = o1 * std::pow(m, 1.0f - o1);
    return static_cast<Engine::RGBA::Component>(o * maxAlpha);
}


ZType& Map::NeighbourHeights::operator[](Orientation ori)
{
    return height[ori.Index()];
}

ZType Map::NeighbourHeights::operator[](Orientation ori) const
{
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
                height += layer.height;
                const auto& material= layer.material;
                auto color = material.get().color;
               
                color.a = ComputeOpacityAtDepth(color.a, layer.height); 
                NeighbourHeights heights = CalculateNeighbourHeights({static_cast<int>(x), static_cast<int>(y), height}, slice);
                AddLayerToMesh(Position(x, y, height), color, heights);                
            }
        }
    }
    assert(mesh.Names().size() == mesh.Triangles().size());
}

Map::NeighbourHeights Map::CalculateNeighbourHeights(Position p, const Slice& centerSlice)
{
    NeighbourHeights result;
    auto [it, amount] = centerSlice.Find(p.Z());
    const auto& layer = *it;

    result[Orientation::up] = p.Z() - amount+ 
        SumHeight(centerSlice.Find(it, std::mem_fn(&Layer::IsTranslucent)));

    ZType bottom(0.0);
    if (it!=centerSlice.begin())
        bottom = p.Z()-amount - (it-1)->height;
    result[Orientation::down] = bottom;
    auto box = GetBounds();
    for(auto ori : Orientations::horizontal)
    {
        auto nx = p.X() + ori.X();
        auto ny = p.Y() + ori.Y();
        if (!box.x[nx] || !box.y[ny])
            result[ori] = bottom;
        else 
        {
            ZType height(0.0);
            const auto& neighbour = SliceAt(nx, ny);
            auto [it, amount] = neighbour.Find(p.Z());
            if (it==neighbour.end())
                height = 0;
            else if (it->IsTranslucent())
                height = p.Z()-amount + 
                        SumHeight(neighbour.FindBackwards(it, std::mem_fn(&Layer::IsOpaque))) - it->height;
            else 
                height  = p.Z() -amount + 
                     SumHeight(neighbour.Find(it, std::mem_fn(&Layer::IsTranslucent)));

            height = std::max(bottom, height);
            result[ori] = height;
        }
    }   
    return result;
}

void Map::AddLayerToMesh(Position pos, Engine::RGBA vertexColor, const NeighbourHeights& neighbours)
{
    Engine::Coordinate backLeft(
        static_cast<double>(pos.X()),
        static_cast<double>(pos.Y()),
        static_cast<double>(pos.Z())
    );

    Engine::Coordinate topVertexCoord[4]={
        backLeft,
        backLeft + Engine::Vector(1,0,0),   // back right
        backLeft + Engine::Vector(1,1,0),   // front right
        backLeft + Engine::Vector(0,1,0)    // front left
    };

    auto name = Index(pos);
    if (neighbours[Orientation::up] <= pos.Z())    // not occluded by layer above
    {
        Engine::Quad top(topVertexCoord);
        if (!vertexColor)
            return;
        top.SetColor(vertexColor);
        top.SetName(name);        
        mesh += top;
    }
    static const unsigned topVertexIndex[7][2] = // indexed by Orientation index
    {
        { 0, 0 },    // None (unused)
        { 0, 0 },    // +Z   (unused)
        { 3, 2 },    // +Y   aka front: front left,  front right (inverted direction from ccw top )  
        { 2, 1 },    // +X   aka right: front right, back right, 
        { 0, 0 },    // -Z   (unused)
        { 1, 0 },    // -Y   aka back   : back right, back left,  
        { 0, 3 }     // -X   aka left: back left, front left,  
    };
    
    for(auto ori: Orientations::horizontal)
    {
        ZType height = pos.Z() - neighbours[ori];
        if (height < 0.0)   // up, let it be drawn by neighbour, for whom it is down, but it's their material and name
            continue; 

        unsigned idx = ori.Index();
        unsigned c0 = topVertexIndex[idx][0],
                c1 =  topVertexIndex[idx][1];
        Engine::Coordinate bottomVertexCoord[2]={
            topVertexCoord[c0] - Engine::Vector(0,0, static_cast<double>(height)),
            topVertexCoord[c1] - Engine::Vector(0,0, static_cast<double>(height)),
        };
        Engine::Quad flank(
            topVertexCoord[c0],
            topVertexCoord[c1],
            bottomVertexCoord[1],
            bottomVertexCoord[0]
        );
        flank.SetColor(vertexColor);
        flank.SetName(name);      
        mesh += flank;      
    }

}

}
