#pragma once

#include "Game/Block.h"
#include "Utility/Range.h"
#include <vector>
#include <span>

namespace Game
{

class Material;
class Block;

class Slice 
{
public:
    Slice() = default;
    explicit Slice(const Block& block);
    Slice(const Slice& other);
    Slice& operator=(const Slice& other);
    
    struct Layer 
    {
        using Amount = Block::Amount;
        using Temperature = Block::Temperature;

        std::reference_wrapper<const Material> material;
        Amount amount; 
        Temperature temperature; 
        
        bool operator==(const Layer& rhs) const;
        float Volume() const;
        float Density() const;  
        bool TryMerge(const Layer& rhs);
    };

    using const_iterator = std::vector<Layer>::const_iterator; 
    inline std::size_t size() const { return layers.size(); }
    inline const_iterator begin() const { return layers.begin(); }
    inline const_iterator end() const { return layers.begin(); }
    inline const Layer& operator[](unsigned idx) { return layers.at(idx); }
    
    Slice& operator+=(const Slice&);
    Slice& operator&=(Engine::Range<double> heigh);
    Slice& operator*=(float mul);
private:
    std::vector<Layer> layers;
};

Slice operator+(const Slice& lhs, const Slice& rhs);
Slice operator&(const Slice& lhs, Engine::Range<double> rng);

}
