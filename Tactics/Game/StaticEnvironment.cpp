#include "stdafx.h"
#include "StaticEnvironment.h"

namespace Game
{
    
StaticEnvironment::StaticEnvironment(const Size& size) :
    m_size(size),
    m_voxels(std::make_unique<Voxel[]>(size.x*size.y*size.z))
{
    for (auto& v : *this)
    {
        double temperature = 290.0;
        v = { &Material::air, temperature, Material::air.Density(PascalPerAtmosphere, temperature) };

    }
}

StaticEnvironment::iterator::iterator(Voxel& v) :
    p(&v)
{
}

StaticEnvironment::iterator& StaticEnvironment::iterator::operator++()
{
    p++;
    return *this;
}
bool StaticEnvironment::iterator::operator==(const StaticEnvironment::iterator& other) const
{
    return p == other.p;
}

bool StaticEnvironment::iterator::operator!=(const iterator& other) const
{
    return p != other.p;
}

StaticEnvironment::iterator::value_type& StaticEnvironment::iterator::operator*() const
{
    return *p;
}

StaticEnvironment::iterator StaticEnvironment::begin()
{
    return iterator(m_voxels[0]);
}

StaticEnvironment::iterator StaticEnvironment::end()
{
    return iterator(m_voxels[m_size.Volume()]);
}


void StaticEnvironment::Fill(const Engine::IVolume& v, const Material& m) 
{
/*
    auto bb = v.GetBoundingBox();
    Box volBB(Position(int(bb.Begin().x), int(bb.Begin().y), int(bb.Begin().z)),
        Position(int(ceil(bb.End().x)), int(ceil(bb.End().y)), int(ceil(bb.End().z))));
    auto envBB = GetBoundingBox();
    auto fillBB = volBB & envBB;
    for (auto& voxel: Section(fillBB))
    {
        
        voxel.material = &m;
        voxel.density = m.Density(m.normalDensity, voxel.temperature);
    }
*/
}

void StaticEnvironment::ApplyForce(const Engine::IVolume& c, const Engine::Vector& v) {}
void StaticEnvironment::ApplyForce(const Engine::Coordinate& c, double force) {}
void StaticEnvironment::Heat(const Engine::Coordinate& c, double energy) {}

void StaticEnvironment::ConnectChange(ChangeSignal::slot_type slot) {}
double StaticEnvironment::Density(const Engine::IVolume& c) const
{
    return 0;
}

double StaticEnvironment::Temperature(const Engine::IVolume& c) const
{
    return 0;
}
Engine::Vector StaticEnvironment::Force(const Engine::IVolume& c) const
{
    return Engine::Vector();
}

const Material* StaticEnvironment::GetMaterial(const Engine::Coordinate& c) const
{
    Position p(int(c.x), int(c.y), int(c.z));
    if (!IsInside(p))
        return nullptr;
    return (*this)[p].material;
}

Box StaticEnvironment::GetBoundingBox() const
{
    return Box(Position(0, 0, 0), m_size);
}
bool StaticEnvironment::IsInside(const Position& pos) const
{
    return GetBoundingBox().Contains(pos);
}

StaticEnvironment::Voxel& StaticEnvironment::operator[](const Position& pos)
{
    return m_voxels[0];
}

const StaticEnvironment::Voxel& StaticEnvironment::operator[](const Position& pos) const
{
    return const_cast<StaticEnvironment*>(this)->operator[](pos);
}

}