#include "stdafx.h"
#include "TreeGrid.h"
#include "Engine/Line.h"
#include "Engine/Volume.h"
#include "Engine/Debug.h"
#include "Physics/BoxIterator.h"
#include <numeric>
#include <sstream>

namespace Physics
{

TreeGrid::TreeGrid(const Engine::Vector& extent, const Grid& grid) :
    root(std::make_unique<Branch>(Size(int(std::round(extent.x / grid.x)), int(std::round(extent.y / grid.y)), int(std::round(extent.z / grid.z)))))
{
    Node::grid = grid;
    root->Fill(Engine::AABox(Engine::Coordinate(0, 0, 0), extent), Position(), Material::vacuum, 0);
}

size_t TreeGrid::Fill(const Engine::IVolume& v, const Material& m, double temperature)
{
    return root->Fill(v, Position(), m, temperature);
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
    Box bounds (root->GetSize());
    if (bounds.Contains(p))
        return root->Get(p);
    else
        return nullptr;
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
    Position p = Node::grid(l.b);
    if (const Leaf* n = Get(p))
    {
        return n->Color();
    }
    else
    {
        return Engine::RGBA();
    }
}

void TreeGrid::Tick(double seconds)
{
}


double TreeGrid::Measure(const Material* material) const
{
    return root->Measure(material);
}

std::wstring TreeGrid::Statistics() const
{
    std::map<unsigned, unsigned> leaves;
    unsigned branches=0;
    root->GetStatistics(leaves, branches);
    std::wstringstream str;
    str << "Branches: " << branches << "(" << branches * sizeof(Branch) / 1024 << "kB) ";
    str << "Leaves:";
    unsigned leafCount = 0;
    for (const auto& leaf : leaves)
    {
        str << "[" << leaf.first << "] = " << leaf.second;
        leafCount += leaf.second;
    }
    str << "(" << leafCount * sizeof(Leaf) / 1024 << "kB) ";
    return str.str();
}


Grid TreeGrid::Node::grid = Grid();

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

unsigned TreeGrid::Branch::Overlap(const Box& box, const Engine::IVolume& v)
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

unsigned TreeGrid::Branch::Fill(const Engine::IVolume& v, const Position& offset, const Material& m, double temperature, const Material* oldMaterial, double oldTemperature)
{
    unsigned filled = 0;
    Box volumeBounds = grid(v.GetBoundingBox()); 
    for (auto idx = 0; idx< nodes.size(); ++idx)
    {
        Physics::Box branchBounds = GetBounds(idx) + offset;

        std::wstringstream boundStr;
        boundStr << branchBounds;

        // None in 
        if ((branchBounds & volumeBounds).Empty())
        {
            if (oldMaterial)
            {  // replace remaining sub node with original material from the old parent
                // there are no sub branched, because the parent was a leaf 
                assert(!nodes[idx]);    // must be growing new branches
                nodes[idx] = std::make_unique<Leaf>(*oldMaterial, oldTemperature);
            }
            else
            {   // keep existing branches if any or keep unfilled
                //Engine::Debug::Log(L"No " + std::wstring(m.name) + L" at " + boundStr.str());
                continue;
            }
        }
        // All in: replace with a leaf full of new material
        else if (AllIn(branchBounds, v))
        {
            //Engine::Debug::Log(std::wstring(m.name) + L" at " + boundStr.str());
            nodes[idx] = std::make_unique<Leaf>(m, temperature);
            filled += branchBounds.Volume();
        }
        else // Partial in, create/reuse a branch for the hybrid materials
        {
            const Material* restMaterial = oldMaterial;
            double restTemperature = oldTemperature;
            // Split up an existing leaf by creating a new branch and setting all unset to the previous material
            if (Leaf* leaf = dynamic_cast<Leaf*>(nodes[idx].get()))
            {
                assert(restMaterial == nullptr);    // This is a leaf so parent was not, so it can't have provided a material
                restMaterial = leaf->GetMaterial();
                restTemperature = leaf->Temperature();
                
                if (restMaterial == &m) //&& TODO, what precision if any? (restTemperature==temperature))
                {   // new material is filled in a leaf that's the same material, so just keep it 
                    filled += Overlap(branchBounds&volumeBounds, v);
                    continue;
                }
            }
            Branch* branch = dynamic_cast<Branch*>(nodes[idx].get());
            if (!branch)
            {
                if (branchBounds.Volume() == 1)
                {   // partial in (Not AllIn) with one grid means it wasn't in at all, just bounding box slightly overlaps
                    if (restMaterial)
                    {
                        nodes[idx] = std::make_unique<Leaf>(*restMaterial, restTemperature);
                        filled += 1;
                    }
                    else
                    {
                        nodes[idx].reset();
                    }
                }
                else
                {   // big enough to make a new branch
                    branch = static_cast<Branch*>((nodes[idx] = std::make_unique<Branch>(branchBounds.Extent())).get());
                }
            }
            if (branch)
            {
                filled += branch->Fill(v, branchBounds.Start(), m, temperature, restMaterial, restTemperature);
                // TODO: possibly optimize and replace branch by leaf if all branch is leaf of the same (from multiple nearby fills)
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
    return Position((size.x+1) >> 1, (size.y+1) >> 1, (size.z+1) >> 1);
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
        return branch->Get(p - GetOffset(idx));
    else
        return nullptr;
}

const TreeGrid::Leaf* TreeGrid::Leaf::Get(const Position& p) const
{
    return this;
}

Size TreeGrid::Branch::GetSize() const
{
    return size;
}

double TreeGrid::Branch::Measure(const Material* material) const
{
    double total = 0;
    for (auto idx = 0; idx< nodes.size(); ++idx)
    {
        auto bounds = GetBounds(idx);
        auto vol = bounds.Volume() * grid.Volume();
        const auto& n = nodes[idx];
        if (const auto* leaf = dynamic_cast<const Leaf*>(n.get()))
        {
            if (leaf->GetMaterial() == material)
                total += vol;
        }
        else if (const auto* branch = dynamic_cast<const Branch*>(n.get()))
        {
            total += branch->Measure(material);
        }
        else if (!material)
        {   // no material in non nodes
            assert(!n);
            total += vol;
        }
    }
    return total;
}

void TreeGrid::Branch::GetStatistics(std::map<unsigned, unsigned>& leaves, unsigned& branches) const
{
    branches += 1;
    for (auto idx = 0; idx < nodes.size(); ++idx)
    {
        auto bounds = GetBounds(idx);
        const auto& n = nodes[idx];
        if (const auto* leaf = dynamic_cast<const Leaf*>(n.get()))
        {
            leaves[bounds.Volume()] += 1;
        }
        else if (const auto* branch = dynamic_cast<const Branch*>(n.get()))
        {
            branch->GetStatistics(leaves, branches);
        }
        else
        {
            assert(!n);
        }
    }
}

}