#include "Game/Slice.h"
#include <cassert>

namespace Game
{

Slice::Slice(const Slice& other) 
    : layers(other.layers)
{
}

Slice::Slice(const Material& mat, Layer::Amount amt, Layer::Temperature temp) :
    Slice{{{mat, amt, temp}}}
{
}

Slice::Slice(std::initializer_list<Layer> layers) : 
    layers(layers)
{
}


void Slice::emplace_back(const Material& material, Layer::Amount amt, Layer::Temperature temp)
{
    layers.emplace_back(material, amt, temp);
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

Slice& Slice::operator&=(Engine::Range<ZType> height)
{
    if (height.begin > height.end) {
        layers.clear();
        return *this;
    }
    if (height.begin < ZType(0)) 
    {
        height.begin = 0;   // TODO could add void here
    }
    auto cutIt = layers.begin();
    ZType progress { 0 };
    auto cutBegin = cutIt;
    while(cutIt!=layers.end())
    {
        if (progress + cutIt->amount >= height.begin) 
        {
            cutBegin = cutIt;
            auto cutProgress = height.begin - progress;
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
            auto cutProgress = height.end - progress;
            cutIt->amount = cutProgress;
            cutEnd = ++cutIt;
            cutIt = layers.erase(cutEnd, layers.end());
            break;
        }
        progress += cutIt->amount;
        ++cutIt;
    }
    // TODO: Could add void/air at the top if progress is still < height.end
    return *this;
}

Slice operator&(const Slice& lhs, Engine::Range<ZType> rng)
{
    Slice result(lhs);
    return result &= rng;
}


Slice& Slice::operator*=(float scale)
{
    for(auto& layer: layers)
    {
        layer.amount *= scale;
    }
    return *this;
}

Slice operator*(const Slice& lhs, float scale)
{
    Slice result(lhs);
    return result *= scale;
}


Engine::Range<ZType> Slice::FindBiggestGasOpening() const
{
    return FindBiggestRange([](const Layer& l)
    {
        return l.IsGas();
    });
}

Engine::Range<ZType> Slice::FindBiggestNonSolidOpening() const
{
    return FindBiggestRange([](const Layer& l)
    {
        return !l.IsSolid();
    });
}

Engine::Range<ZType> Slice::FindBiggestRange(std::function<bool(const Layer&)> predicate) const
{
    auto result = Engine::Range<ZType>::empty();
    Engine::Range<ZType> current = Engine::Range<ZType>::empty();
    ZType progress = 0;
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
            current = Engine::Range<ZType>::empty();
        }
        progress += layer.amount;
    }
    current.end = progress; 

    if (current.Size() > result.Size())
        result = current;

    return result;

}


}
