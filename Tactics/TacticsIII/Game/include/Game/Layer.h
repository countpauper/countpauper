#pragma once 

#include "Utility/FixedPoint.h"
#include "Game/Material.h"
#include "Game/Orientation.h"
#include "Utility/Concepts.h"
#include <functional>
#include <numeric>

namespace Game
{




struct Layer 
{
    using Height = Engine::FixedPoint<8, uint16_t>;   
    using Temperature = Engine::FixedPoint<3,uint16_t>;        
    using Flow = Engine::FixedPoint<10,short>;
    std::reference_wrapper<const Material> material;
    
    Height height;                            // meter 
    Temperature temperature;                  // kelvin
    std::array<Flow,3> flow = {0.0, 0.0, 0.0};// meter/second (or liter/sec/m2) per Orientation axis index-1

    bool operator==(const Layer& rhs) const;
    float Volume() const;
    float Density() const;  
    bool IsGas() const;
    bool IsSolid() const;
    bool IsOpaque() const;
    bool IsTranslucent() const;
    void AddFlow(Orientation dir, Flow df);
    Flow GetFlow(Orientation dir) const;
    using Predicate = std::function<bool(const Layer&)>;
    bool TryMerge(const Layer& rhs);
};

template<typename R>
concept RangeOfLayer = RangeOf<R, Layer>;

Layer::Height SumHeight(RangeOfLayer auto&& range)
{
    return std::accumulate(std::ranges::begin(range), std::ranges::end(range), Layer::Height(0.0),[](Layer::Height sum, const Layer& layer)
    {
        return sum + layer.height;
    });
}

std::ostream& operator<<(std::ostream& os, const Layer& layer);

}