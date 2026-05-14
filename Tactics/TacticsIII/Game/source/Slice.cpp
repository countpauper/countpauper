#include "Game/Slice.h"
#include "Utility/Assert.h"

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
    if (!layers.empty())
    {
        if (layers.back().TryMerge(Layer{material, amt, temp}))
            return;
    }
    layers.emplace_back(material, amt, temp);
}


std::pair<Slice::const_iterator, Layer::Amount> Slice::Find(ZType height) const
{
    Layer::Amount progress{0.0};
    for(auto it = begin(); it!=end(); ++it)
    {
        if (progress + it->amount > height)
            return std::make_pair(it, height - progress);
        progress += it->amount;
    }
    return std::make_pair(end(), Layer::Amount());
}

std::pair<Slice::iterator, Layer::Amount> Slice::Find(ZType height)
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
    Layer::Amount progress { static_cast<Layer::Amount::IntegerType>(0) };
    auto cutBegin = cutIt;
    while(cutIt!=layers.end())
    {
        if (progress + cutIt->amount >= height.begin) 
        {
            auto cutProgress = height.begin - progress;
            progress += cutProgress;
            cutIt->amount -= cutProgress;
            break;
        }
        progress += cutIt->amount;
        cutIt = layers.erase(cutIt);
    }

    auto cutEnd = cutIt;
    while(cutIt!=layers.end()) 
    {
        if (progress + cutIt->amount >= height.end)
        {
            auto cutProgress = height.end - progress;
            cutIt->amount = cutProgress;
            progress += cutProgress;
            cutEnd = ++cutIt;
            cutIt = layers.erase(cutEnd, layers.end());
            break;
        }
        progress += cutIt->amount;
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
        progress += layer.amount;
    }
    current.end = progress; 

    if (current.Size() > result.Size())
        result = current;

    return result;

}

Slice::iterator Slice::Fill(Engine::Range<ZType> height, const Material& material, Layer::Temperature temperature)
{
    auto[it, amount] = Find(height.begin);
    Layer::Amount displaced = 0.0;    // the volume removed to make room for the height
    Layer::Amount level = SumAmount(std::ranges::subrange(begin(), it));
    if (it!=layers.end() && amount!=0.0)
    {
        displaced += it->amount - amount;
        level += it->amount;
        it->amount = amount;
        if (displaced > height.Size())
        {   // split this overlapping layer 
            it = layers.emplace(it+1, it->material, displaced - height.Size(), it->temperature);
            return layers.emplace(it, material, height.Size(), temperature);
        }
        ++it;
        // TODO: later (as an option?) increase pressure by keeping the mass in a reduced volume 
    }
    auto replaceStart = it;
    while(it!= layers.end() && level + it->amount <= height.end)
    {
        level+=it->amount;
        displaced += it->amount;
        ++it;
    }
    // TODO: what to do with the mass of the replaced? Compress it into the layer above in some balance?
    it = layers.erase(replaceStart, it);
    if (it!=layers.end())
    {
        it->amount -= height.Size() - displaced;
        // TODO increase pressure 
    }
    return layers.emplace(it, material, height.Size(), temperature);

}

}
