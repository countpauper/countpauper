#include "stdafx.h"
#include "TreeGrid.h"
#include "Engine/Line.h"
#include "Engine/Volume.h"
#include <numeric>

namespace Physics
{

TreeGrid::TreeGrid(const Engine::Vector& extent, const Engine::Vector& grid) :
    root(std::make_unique<Branch>(Size(int(std::round(extent.x / grid.x)), int(std::round(extent.y / grid.y)), int(std::round(extent.z / grid.z)))))
{
    Node::grid = Grid(grid);
    root->Fill(Engine::AABox(Engine::Coordinate(0, 0, 0), extent), Position(), Material::vacuum, 0);
}

size_t TreeGrid::Fill(const Engine::IVolume& v, const Material& m, double temperature)
{
    root->Fill(v, Position(), m, temperature);
    return 0;
}

void TreeGrid::ApplyForce(const Engine::IVolume& c, const Engine::Vector& v) {}
void TreeGrid::ApplyForce(const Engine::Coordinate& c, double force) {}
void TreeGrid::Heat(const Engine::Coordinate& c, double energy) {}

void TreeGrid::ConnectChange(ChangeSignal::slot_type slot) {}

double TreeGrid::Density(const Engine::IVolume& c) const
{
    return 0.0;
}

double TreeGrid::Temperature(const Engine::IVolume& v) const
{
    auto temperatureVolume = root->GetTemperature(v, Position());
    if (temperatureVolume.second)
        return temperatureVolume.first / temperatureVolume.second;
    else
        return 0.0; // vacuum
}

Engine::Vector TreeGrid::Force(const Engine::IVolume& c) const
{
    return Engine::Vector();
}

const TreeGrid::Leaf* TreeGrid::Get(const Position& p) const
{
    return root->Get(p);
}

const Material* TreeGrid::GetMaterial(const Engine::Coordinate& c) const
{
    Position p=Node::grid(c);
    if (const Leaf* n = Get(p))
    {
        return n->GetMaterial();
    }
    else
    {
        return nullptr;
    }
}

Engine::RGBA TreeGrid::Color(const Engine::Line& l) const
{
    Position p = Node::grid(l.a);
    if (const Leaf* n = Get(p))
    {
        return n->GetMaterial()->color;
    }
    else
    {
        return Engine::RGBA();
    }
}

void TreeGrid::Tick(double seconds)
{
}

TreeGrid::Leaf::Leaf():
    material(0),
    amount(0),
    hot(0),
    temperature(0)
{
}

TreeGrid::Leaf::Leaf(const Material& m, double temperature)
{
    Set(&m);
    SetTemperature(temperature);
}

Grid TreeGrid::Node::grid = Grid();

const Material* TreeGrid::Leaf::mats[] =
{
    &Material::vacuum,
    &Material::air,
    &Material::water,
    &Material::soil,
    &Material::stone,
    nullptr
};

void TreeGrid::Leaf::Set(const Material* newMat)
{
    for (material = 0; mats[material]; material++)
    {
        if (mats[material] == newMat)
            return;
    }
    throw std::runtime_error("Material needs to be a standard one");
}


const TreeGrid::Leaf* TreeGrid::Leaf::Get(const Position& p) const
{
    return this;
}

const Material* TreeGrid::Leaf::GetMaterial() const
{

    return mats[material];
}

double TreeGrid::Leaf::Temperature() const
{
    if (hot)
    {
        return HotOffset + temperature * HotTGradient;
    }
    else
    {
        return temperature * ColdTGradient;
    }
}

void TreeGrid::Leaf::SetTemperature(double t)
{
    int it = int(std::floor(t));
    if (t > HotOffset)
    {
        hot = 1;
        temperature = (it - HotOffset) / HotTGradient;
    }
    else
    {
        hot = 0;
        temperature = it / ColdTGradient;
    }
}

double TreeGrid::Leaf::Density() const
{
    return GetMaterial()->normalDensity;    // TODO: temperature? 
}


TreeGrid::Branch::Branch(const Size& size) :
    size(size)
{

}

unsigned TreeGrid::Branch::GetIndex(const Position& p) const
{
    Position m = Pivot();
    return (1 * (p.x >= m.x)) + (2 * (p.y >= m.y)) + (4 * (p.z >= m.z));
}

Position TreeGrid::Branch::GetOffset(unsigned index) const
{
    Position p = Pivot();
    return Position(index & 1 ? p.x : 0,
        index & 2 ? p.y : 0,
        index & 4 ? p.z : 0);
}

Box TreeGrid::Branch::GetBounds(unsigned index) const
{
    Position p = Pivot();
    return Box(index & 1 ? Engine::Range<int>(p.x, size.x) : Engine::Range<int>(0, p.x),
        index & 2 ? Engine::Range<int>(p.y, size.y) : Engine::Range<int>(0, p.y),
        index & 4 ? Engine::Range<int>(p.z, size.z) : Engine::Range<int>(0, p.z));
}


bool TreeGrid::Branch::AllIn(const Box& box, const Engine::IVolume& v)
{
    for (BoxIterator i(box); i != i.end(); ++i)
    {
        if (!v.Contains(grid.Center(*i)))
        {
            return false;
        }
    }
    return true;
}

double TreeGrid::Branch::Overlap(const Box& box, const Engine::IVolume& v)
{
    auto count = 0;
    for (BoxIterator i(box); i != i.end(); ++i)
    {
        if (v.Contains(grid.Center(*i)))
        {
            count += 1;
        }
    }
    return count;
}

unsigned TreeGrid::Branch::Fill(const Engine::IVolume& v, const Position& offset, const Material& m, double temperature)
{
    unsigned filled = 0;
    Box volumeBounds = grid(v.GetBoundingBox());
    for (auto idx = 0; idx< nodes.size(); ++idx)
    {
        Physics::Box branchBounds = GetBounds(idx) + offset;
       
        if ((branchBounds & volumeBounds).Empty())
            continue;
        if (AllIn(branchBounds, v))
        {
            nodes[idx] = std::make_unique<Leaf>(m, temperature);
            filled += branchBounds.Volume();
        }
        else
        {
            Branch* branch = dynamic_cast<Branch*>(nodes[idx].get());
            if (!branch)
            {
                nodes[idx] = std::make_unique<Branch>(branchBounds.Extent());
                branch = static_cast<Branch*>(nodes[idx].get());
            }
            if (branch)
            {
                filled += branch->Fill(v, offset + branchBounds.Start(), m, temperature);
            }
        }

    }
    return filled;
}


std::pair<double, double> TreeGrid::Branch::GetTemperature(const Engine::IVolume& v, const Position& offset) const
{
    double temperature = 0;
    double totalVolume = 0;
    Box volumeBounds = grid(v.GetBoundingBox());
    for (auto idx = 0; idx< nodes.size(); ++idx)
    {
        Physics::Box branchBounds = GetBounds(idx) + offset;
        if ((branchBounds & volumeBounds).Empty())
            continue;
        auto* node = nodes[idx].get();
        if (!node)
        {
            continue;
        }
        else if (TreeGrid::Leaf* leaf = dynamic_cast<Leaf*>(node))
        {
            auto volume = Overlap(branchBounds, v);
            totalVolume += volume;
            temperature += leaf->Temperature() * volume;
        }
        else if (TreeGrid::Branch* branch = dynamic_cast<Branch*>(node))
        {
            auto tempVolume = branch->GetTemperature(v, branchBounds.Start());
            temperature += tempVolume.first;
            totalVolume += tempVolume.second;
        }
        else
        {
            assert(false);  // what is it?
        }

    }
    return std::make_pair(temperature, totalVolume);

}

Position TreeGrid::Branch::Pivot() const
{   
    return Position(size.x >> 1, size.y >> 1, size.z >> 1);
/*        // TODO can maybe be optimized by equalizing the size in the constructor, but then the maps size might get much bigger
        // or by caching one coordinate in the constructor
        // can also just take the middle, we have uncubed nodes anyway 
        int v= std::max(size.x, std::max(size.y, size.z)) -1;
    if (v<=0)
        return Position();

    unsigned r = 0;
    while (v >>= 1) 
    {
        r++;
    }
    return Position(1 << r, 1 << r, 1 << r);
*/
}

const TreeGrid::Leaf* TreeGrid::Branch::Get(const Position& p) const
{
    auto idx = GetIndex(p);
    if (auto branch = nodes[idx].get())
        return branch->Get(p);
    else
        return nullptr;
}


}