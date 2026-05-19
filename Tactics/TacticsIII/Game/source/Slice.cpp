#include "Game/Slice.h"
#include "Utility/Assert.h"

namespace Game
{

Slice::Slice(const Slice& other) 
    : layers(other.layers)
{
}

Slice::Slice(const Material& mat, Layer::Height height, Layer::Temperature temp) :
    Slice{{{mat, height, temp}}}
{
}

Slice::Slice(std::initializer_list<Layer> layers) : 
    layers(layers)
{
}


void Slice::emplace_back(const Material& material, Layer::Height height, Layer::Temperature temp)
{
    if (!layers.empty())
    {
        if (layers.back().TryMerge(Layer{material, height, temp}))
            return;
    }
    layers.emplace_back(material, height, temp);
}


std::pair<Slice::const_iterator, Layer::Height> Slice::Find(ZType height) const
{
    Layer::Height progress{0.0};
    for(auto it = begin(); it!=end(); ++it)
    {
        if (progress + it->height > height)
            return std::make_pair(it, height - progress);
        progress += it->height;
    }
    return std::make_pair(end(), Layer::Height());
}

std::pair<Slice::iterator, Layer::Height> Slice::Find(ZType height)
{   // avoid duplicating Find algorithm by "const casting" the iterator 
    auto[cit, amount] = const_cast<const Slice*>(this)->Find(height);
    unsigned idx = cit - const_cast<const Slice*>(this)->begin();
    return std::make_pair(layers.begin() + idx, amount);
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
    auto cutIt = layers.begin();
    if (height.begin < ZType(0))
    {
        cutIt = layers.insert(cutIt, {Material::vacuum, -height.begin, 0.0});
        ++cutIt;
        height.begin = ZType(0);
    }
    Layer::Height progress { static_cast<Layer::Height::IntegerType>(0) };
    auto cutBegin = cutIt;
    while(cutIt!=layers.end())
    {
        if (progress + cutIt->height >= height.begin) 
        {
            auto cutProgress = height.begin - progress;
            progress += cutProgress;
            cutIt->height -= cutProgress;
            break;
        }
        progress += cutIt->height;
        cutIt = layers.erase(cutIt);
    }

    auto cutEnd = cutIt;
    while(cutIt!=layers.end()) 
    {
        if (progress + cutIt->height >= height.end)
        {
            auto cutProgress = height.end - progress;
            cutIt->height = cutProgress;
            progress += cutProgress;
            cutEnd = ++cutIt;
            cutIt = layers.erase(cutEnd, layers.end());
            break;
        }
        progress += cutIt->height;
        ++cutIt;
    }
    if (progress < height.end)
    {
        (*this)+=Slice({Material::vacuum, height.end - progress, 0.0});
    }
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
        layer.height *= scale;
    }
    return *this;
}

Slice operator*(const Slice& lhs, float scale)
{
    Slice result(lhs);
    return result *= scale;
}


Engine::Range<ZType> Slice::FindBiggestOpening() const
{
    return FindBiggestRange([](const Layer& l)
    {
        return l.IsCompressible();
    });
}

Engine::Range<ZType> Slice::FindBiggestNonSolidOpening() const
{
    return FindBiggestRange(std::mem_fn(&Layer::IsGas));
}

Engine::Range<ZType> Slice::FindBiggestRange(std::function<bool(const Layer&)> predicate) const
{
    auto result = Engine::Range<ZType>::empty();
    ZType progress = 0;
    Engine::Range<ZType> current = result;
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
        progress += layer.height;
    }
    current.end = progress; 

    if (current.Size() > result.Size())
        result = current;

    return result;

}

Slice::iterator Slice::Fill(Engine::Range<ZType> height, const Material& material, Layer::Temperature temperature)
{
    auto[it, offset] = Find(height.begin);
    Layer::Height displaced = 0.0;    // the volume removed to make room for the height
    Layer::Height level = SumHeight(std::ranges::subrange(begin(), it));
    if (it!=layers.end() && offset!=0.0)
    {
        displaced += it->height - offset;
        level += it->height;
        it->height = offset;
        if (displaced > height.Size())
        {   // split this overlapping layer 
            it = layers.emplace(it+1, it->material, displaced - height.Size(), it->temperature);
            return layers.emplace(it, material, height.Size(), temperature);
        }
        ++it;
        // TODO: later (as an option?) increase pressure by keeping the mass in a reduced volume 
    }
    auto replaceStart = it;
    while(it!= layers.end() && level + it->height <= height.end)
    {
        level+=it->height;
        displaced += it->height;
        ++it;
    }
    // TODO: what to do with the mass of the replaced? Compress it into the layer above in some balance?
    it = layers.erase(replaceStart, it);
    if (it!=layers.end())
    {
        it->height -= height.Size() - displaced;
        // TODO increase pressure 
    }
    return layers.emplace(it, material, height.Size(), temperature);

}

}
