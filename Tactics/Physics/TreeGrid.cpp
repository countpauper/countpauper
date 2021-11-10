#include "stdafx.h"
#include "TreeGrid.h"
#include "Engine/Line.h"
#include "Engine/Volume.h"

namespace Physics
{

TreeGrid::TreeGrid(const Engine::Vector& extent, const Engine::Vector& grid) :
    grid(grid),
    size(int(std::round(extent.x / grid.x)), int(std::round(extent.y / grid.y)), int(std::round(extent.z / grid.z))),
    data(this->size.x * this->size.y * this->size.z)
{
}

size_t TreeGrid::Fill(const Engine::IVolume& v, const Material& m, double temperature)
{
    auto aabb = v.GetBoundingBox();
    Box bounds = Box(grid(aabb.Begin()),grid(aabb.End())) & Bounds();
    for (iterator i = iterator(*this, bounds); bool(i); ++i)
    {
        if (v.Contains(grid.Center((*i).first)))
        {
            (*i).second.Set(&m);
            (*i).second.SetTemperature(temperature);
        }
    }
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

double TreeGrid::Temperature(const Engine::IVolume& c) const
{
    double kineticEnergy = 0;
    double overlapVolume = 0;
    auto aabb = c.GetBoundingBox();
    Box bounds = Box(grid(aabb.Begin()), grid(aabb.End())) & Bounds();
    for (const_iterator i = const_iterator(*this, bounds); bool(i); ++i)
    {
        if (c.Contains(grid.Center((*i).first)))
        {
            kineticEnergy += (*i).second.Temperature();
            overlapVolume += 1.0;   // TODO: if it could check overlap of a grid with volume, then it could divide by c.Volume() (if available)
        }
    }
    return kineticEnergy / overlapVolume;
}

Engine::Vector TreeGrid::Force(const Engine::IVolume& c) const
{
    return Engine::Vector();
}

Box TreeGrid::Bounds() const
{
    return Box(size);
}

bool TreeGrid::Inside(const Position& p) const
{
    return Bounds().Contains(p);
}

unsigned TreeGrid::Index(const Position& p) const
{
    return p.x + (p.y * size.x) + (p.z * size.x * size.y);
}


const TreeGrid::Node* TreeGrid::Get(const Position& p) const
{
    if (Inside(p))
    {
        return &data.at(Index(p));
    }
    else
    {
        return nullptr;
    }
}

const struct TreeGrid::Node& TreeGrid::operator[](const Position& p) const
{
    return data.at(Index(p));
}

struct TreeGrid::Node& TreeGrid::operator[](const Position& p)
{
    return data.at(Index(p));
}


const Material* TreeGrid::GetMaterial(const Engine::Coordinate& c) const
{
    Position p=grid(c);
    if (const Node* n = Get(p))
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
    Position p = grid(l.a);
    if (const Node* n = Get(p))
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

TreeGrid::Node::Node():
    material(0),
    amount(0),
    hot(0),
    temperature(0)
{
}

const Material* TreeGrid::Node::mats[] =
{
    &Material::vacuum,
    &Material::air,
    &Material::water,
    &Material::soil,
    &Material::stone,
    nullptr
};

void TreeGrid::Node::Set(const Material* newMat)
{
    for (material = 0; mats[material]; material++)
    {
        if (mats[material] == newMat)
            return;
    }
    throw std::runtime_error("Material needs to be a standard one");
}

const Material* TreeGrid::Node::GetMaterial() const
{

    return mats[material];
}

double TreeGrid::Node::Temperature() const
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

void TreeGrid::Node::SetTemperature(double t)
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
double TreeGrid::Node::Density() const
{
    return GetMaterial()->normalDensity;    // TODO: temperature? 
}

}