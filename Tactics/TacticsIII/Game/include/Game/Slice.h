#pragma once

#include "Utility/Range.h"
#include "Game/Position.h"
#include "Game/Layer.h"

#include <vector>
#include <span>
#include <initializer_list>

namespace Game
{

class Material;

class Slice 
{
public:
    Slice() = default;
    Slice(const Slice& other);
    Slice& operator=(const Slice& other);
    Slice(const Material& material, Layer::Amount amt=1.0, Layer::Temperature temp=300.0);
    Slice(std::initializer_list<Layer> layers);

    void emplace_back(const Material& material, Layer::Amount amt, Layer::Temperature temp);

    using const_iterator = std::vector<Layer>::const_iterator; 
    inline std::size_t size() const { return layers.size(); }
    inline const_iterator begin() const { return layers.begin(); }
    inline const_iterator end() const { return layers.end(); }
    inline const Layer& operator[](unsigned idx) { return layers.at(idx); }
    using value_type = Layer; 
    
    Slice& operator+=(const Slice&);
    Slice& operator&=(Engine::Range<ZType> heigh);
    Slice& operator*=(float scale);

    Engine::Range<ZType> FindBiggestGasOpening() const;
    Engine::Range<ZType> FindBiggestNonSolidOpening() const;
    Engine::Range<ZType> FindBiggestRange(std::function<bool(const Layer&)> predicate) const;

private:
    std::vector<Layer> layers;
};

Slice operator+(const Slice& lhs, const Slice& rhs);
Slice operator&(const Slice& lhs, Engine::Range<ZType> rng);
Slice operator*(const Slice& lhs, float scale);




}
