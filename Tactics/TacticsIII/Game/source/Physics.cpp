#include "Game/Physics.h"
#include "Game/Slice.h"

namespace Game 
{

Gravity::Gravity(float gravity) :
    gravity(gravity)
{
}

void Gravity::operator()(float dt, Slice& slice) const
{
    float dv = dt * gravity; // meter per second
    for(auto& layer: slice | std::views::reverse)
    {
        layer.AddFlow(Orientation::down, dv);        
    }

}

}