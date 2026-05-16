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
    using Height = Engine::FixedPoint<8, uint16_t>;     // Range 256m, resolution about 3.906mm
    using Temperature = Engine::FixedPoint<3,uint16_t>; // range 8192K, resolution 125mK        
    using Flow = Engine::FixedPoint<8,short>;           // range +/- 128m/sec, resulution about 4mm/sec 
    using Density = Engine::FixedPoint<18,uint32_t>;    // range 16384 @ resolution about 7.6 mg/m3

    std::reference_wrapper<const Material> material;
    Height height;                                  // meter 
    Temperature temperature;                        // kelvin 
    Density density;                                // kg/m3 
    std::array<Flow,3> flow = {0.0, 0.0, 0.0};      // meter/second (or liter/sec/m2) per Orientation axis index-1
    Layer(const Material& material=Material::vacuum, Height height=1.0, Temperature temperature = 300.0, float pressure=atmosphericPressure);

    bool operator==(const Layer& rhs) const;
    float Volume() const;
    float Mass() const;
    float Viscosity() const;
    Flow GetFlow(Orientation dir) const;

    bool IsGas() const;
    bool IsSolid() const;
    bool IsOpaque() const;
    bool IsTranslucent() const;

    void AddFlow(Orientation dir, Flow df);

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