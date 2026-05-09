#include "Game/Surface.h"
#include "Game/Material.h"
#include <GL/gl.h>

namespace Game 
{

Surface::Surface(const Material& material, float size) :
    material(material),
    size(size)
{
}

void Surface::Render() const 
{
    material.color.Render();
    glPushMatrix();
    glScalef(size, size, 1.0f);
    mesh.Render();
    glPopMatrix();
}

std::pair<double, std::uint32_t> Surface::Intersection(const Engine::Line& line) const 
{ 
    return {std::numeric_limits<double>::infinity(), 0};
}

void Surface::Move(const class World& world, Position destination) 
{ 

}

Position Surface::GetPosition() const 
{ 
    return Position(
        std::floor(coordinate.X()),
        std::floor(coordinate.Y()),
        coordinate.Z()
    );
}

Size Surface::GetSize() const 
{ 
    return Size{static_cast<int>(std::round(size)), static_cast<int>(std::round(size)), 0.0f};
}

const Engine::Object& Surface::GetAppearance() const 
{    
    return *this;
}

Statted& Surface::GetStats() 
{ 
    return stats;
}

const Statted& Surface::GetStats() const 
{ 
    return stats;
}

Counted& Surface::GetCounts() 
{ 
    return counts;
}

const Counted& Surface::GetCounts() const 
{ 
    return counts;
}

const class Equipped& Surface::GetEquipment() const 
{ 
    return equipment;
}

class Equipped& Surface::GetEquipment() 
{ 
    return equipment;
}

class Conditions& Surface::GetConditions() 
{ 
    return conditions;
}

const class Conditions& Surface::GetConditions() const 
{ 
    return conditions;

}


}