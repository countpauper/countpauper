#pragma once

#include "Utility/Range.h"
#include "Game/Position.h"
#include "Game/Layer.h"

#include <vector>
#include <span>
#include <initializer_list>
#include <iterator>

namespace Game
{

class Material;

class Slice 
{
public:
    Slice() = default;
    Slice(const Slice& other);
    Slice& operator=(const Slice& other);
    Slice(const Material& material, Layer::Height height=1.0, Layer::Temperature temp=300.0);
    Slice(std::initializer_list<Layer> layers);

    void emplace_back(const Material& material, Layer::Height height, Layer::Temperature temp);
    Layer pop_front();

    using Layers = std::vector<Layer>;
    using const_iterator =Layers::const_iterator; 
    using const_reverse_iterator = Layers::const_reverse_iterator;
    using iterator = Layers::iterator;
    using value_type = Layer; 
    
    inline std::size_t size() const { return layers.size(); }
    inline const_iterator begin() const { return layers.begin(); }
    inline const_iterator end() const { return layers.end(); }
    inline iterator begin() { return layers.begin(); }
    inline iterator end() { return layers.end(); }    
    inline const_reverse_iterator rbegin() const { return layers.rbegin(); }
    inline const_reverse_iterator rend() const { return layers.rend(); } 
    // Return the iterator of the layer at 'height'  and the amount that height is above the bottom of that layer
    std::pair<const_iterator, Layer::Height> Find(ZType height) const;
    std::pair<iterator,Layer::Height> Find(ZType height);
    inline const Layer& operator[](unsigned idx) const { return layers.at(idx); }
    inline Layer& operator[](unsigned idx) { return layers.at(idx); }

    
    Slice& operator+=(const Slice&);
    Slice& operator&=(Engine::Range<ZType> heigh);
    Slice& operator*=(float scale);

    Engine::Range<ZType> FindBiggestOpening() const;
    Engine::Range<ZType> FindBiggestNonSolidOpening() const;
    Engine::Range<ZType> FindBiggestRange(Layer::Predicate predicate) const;

    auto Find(const_iterator from, Layer::Predicate to) const 
    {
        return std::ranges::subrange(from, std::find_if(from, layers.end(), to));
    }

    auto FindBackwards(const_iterator from, Layer::Predicate to) const 
    {
        
        if (from == layers.end())
            return std::ranges::subrange(layers.end(), layers.end());
        auto it = from;
        for(; to(*it); --it)
        {
            if (it==layers.begin())
                return std::ranges::subrange(it, from+1);                
        }
        return  std::ranges::subrange(it+1, from+1);
    }
    iterator Fill(Engine::Range<ZType> height, const Material& material, Layer::Temperature temperature);
private:
    Layers layers;
};

Slice operator+(const Slice& lhs, const Slice& rhs);
Slice operator&(const Slice& lhs, Engine::Range<ZType> rng);
Slice operator*(const Slice& lhs, float scale);




}
