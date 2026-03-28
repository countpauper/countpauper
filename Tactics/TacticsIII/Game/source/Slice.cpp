#include "Game/Slice.h"
#include "Game/Block.h"
#include <cassert>

namespace Game
{


Slice::Slice(const Block& block)
{
    if (block.stone)
    {
        layers.emplace_back(Layer{Material::stone, block.stone, block.temperature});
    }
    if (block.water)
    {
        layers.emplace_back(Layer(Material::water, block.water, block.temperature));
    }
    assert(!block.vegetation);  // TODO should add to water volume if any or subtract from air. Not sure what to do with this as an obstacle/cover 
    /// maybe it's layer needs a 'vegetation density' amount 
    if (block.air)
    {
        layers.emplace_back(Layer(Material::air, block.air, block.temperature));
    }
}

Slice::Slice(const Slice& other) 
    : layers(other.layers)
{
}


float Slice::Layer::Volume() const
{
    return amount * 100.0f;
}

float Slice::Layer::Density() const
{
    return 1.0f;
}

bool Slice::Layer::TryMerge(const Slice::Layer& rhs) 
{
    if (material.get() != rhs.material.get())
        return false;
    if (this->temperature != rhs.temperature) 
        return false;   // Maybe is close enough (like less than 1 celsius) still mix 
    amount += rhs.amount;
    return true;
}

Slice& Slice::operator=(const Slice& rhs)
{
    layers = rhs.layers;
    return *this;
}
 
Slice& Slice::operator+=(const Slice& rhs)
{
    if (layers.empty()) 
    {
        layers = rhs.layers;
        return *this;
    }
    auto start = rhs.layers.begin();
    while (start!= rhs.layers.end() && 
            layers.back().TryMerge(*start))
    {
        ++start;
    }   
    layers.insert(end(), start, rhs.layers.end());
    return *this;
}


Slice operator+(const Slice& lhs, const Slice& rhs)
{
    Slice result(lhs);
    return result += rhs;
}

Slice& Slice::operator&=(Engine::Range<double> height)
{
    return *this;
}

Slice& Slice::operator*=(float mul)
{
    return *this;
}


}
