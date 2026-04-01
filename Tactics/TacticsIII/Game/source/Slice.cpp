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


bool Slice::Layer::operator==(const Slice::Layer& rhs) const
{
    if (material.get() != rhs.material.get())
        return false;
    if (amount != rhs.amount)
        return false;
    if (temperature != rhs.temperature)
        return false;
    return true;
}

float Slice::Layer::Volume() const
{
    return amount * 100.0f;
}

float Slice::Layer::Density() const
{
    return material.get().Density(temperature);
}

 
bool Slice::Layer::IsGas() const
{
    return material.get().IsGas(temperature);
}

bool Slice::Layer::IsSolid() const
{
    return material.get().IsSolid(temperature);
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
    layers.insert(layers.end(), start, rhs.layers.end());
    return *this;
}


Slice operator+(const Slice& lhs, const Slice& rhs)
{
    Slice result(lhs);
    return result += rhs;
}

Slice& Slice::operator&=(Engine::Range<double> height)
{
    if (height.begin > height.end) {
        layers.clear();
        return *this;
    }
    auto cutIt = layers.begin();
    double progress = 0.0;
    auto cutBegin = cutIt;
    while(cutIt!=layers.end())
    {
        if (progress + cutIt->amount >= height.begin) 
        {
            cutBegin = cutIt;
            double cutProgress = height.begin - progress;
            progress += cutProgress;
            cutBegin->amount -= cutProgress;
            break;
        }
        progress += cutIt->amount;
        ++cutIt;
    }
    cutIt = layers.erase(layers.begin(), cutBegin);


    auto cutEnd = cutIt;
    while(cutIt!=layers.end()) 
    {
        if (progress + cutIt->amount >= height.end)
        {
            double cutProgress = height.end - progress;
            cutIt->amount = cutProgress;
            cutEnd = ++cutIt;
            break;
        }
        progress += cutIt->amount;
        ++cutIt;
    }
    cutIt = layers.erase(cutEnd, layers.end());
    return *this;
}

Slice operator&(const Slice& lhs, Engine::Range<double> rng)
{
    Slice result(lhs);
    return result &= rng;
}


Slice& Slice::operator*=(double scale)
{
    for(auto& layer: layers)
    {
        layer.amount *= scale;
    }
    return *this;
}

Slice operator*(const Slice& lhs, double scale)
{
    Slice result(lhs);
    return result *= scale;
}


Engine::Range<double> Slice::FindGasOpening() const
{
    return FindRange([](const Layer& l)
    {
        return l.IsGas();
    });
}

Engine::Range<double> Slice::FindNonSolidOpening() const
{
    return FindRange([](const Layer& l)
    {
        return !l.IsSolid();
    });
}

Engine::Range<double> Slice::FindRange(std::function<bool(const Slice::Layer&)> predicate) const
{
    auto result = Engine::Range<double>::empty();
    Engine::Range<double> current = Engine::Range<double>::empty();
    double progress = 0;
    for(const auto& layer : layers)
    {
        if (predicate(layer))
        {
            current.begin = std::min(progress, current.begin);
        }
        else if (!predicate(layer))
        {
            current.end = progress; 
            if (current.Size() > result.Size())
                result = current;
            current = Engine::Range<double>::empty();
        }
        progress += layer.amount;
    }
    current.end = progress; 

    if (current.Size() > result.Size())
        result = current;

    return result;

}


}
