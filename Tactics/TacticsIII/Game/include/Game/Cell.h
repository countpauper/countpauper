#pragma once 

#include "Utility/FixedPoint.h"
#include "Game/Material.h"
#include "Game/Orientation.h"
#include "Game/Position.h"  // just for ZType
#include "Utility/Concepts.h"
#include <functional>
#include <numeric>

namespace Game
{

struct Cell 
{
    using Height = Engine::FixedPoint<8, uint16_t>;     // Range 256m, resolution about 3.906mm
    using Temperature = Engine::FixedPoint<3,uint16_t>; // range 8192K, resolution 125mK        
    using Flow = Engine::FixedPoint<8,short>;           // range +/- 128m/sec, resulution about 4mm/sec 
    using Pressure = uint32_t;                          // range 4.29 GPa @ resolution about 1 Pa
    std::reference_wrapper<const Material> material;
    Height height;                                  // meter 
    Temperature temperature;                        // kelvin 
    std::array<Flow,3> flow = {0.0, 0.0, 0.0};      // meter/second (or liter/sec/m2) per Orientation axis index-1
    Pressure pressure;                              // Pascal
    Cell(const Material& material=Material::vacuum, Height height=1.0, Temperature temperature = 300.0, Pressure pressure=atmosphericPressure);

    bool operator==(const Cell& rhs) const;
    float Volume() const;
    float Mass() const;
    float Density() const;
    float Viscosity() const;
    Flow GetFlow(Orientation dir) const;
    float GetPressure(Height height, float gravity) const;

    bool IsGas() const;
    bool IsCompressible() const;
    bool IsLiquid() const;
    bool IsSolid() const;
    bool IsOpaque() const;
    bool IsTranslucent() const;

    void AccelerateFlow(Orientation dir, Flow df);
    void Heat(float degrees);

    using Predicate = std::function<bool(const Cell&)>;
    bool TryMerge(const Cell& rhs);
};

template<typename R>
concept RangeOfCell = RangeOf<R, Cell>;

Cell::Height SumHeight(RangeOfCell auto&& range)
{
    return std::accumulate(std::ranges::begin(range), std::ranges::end(range), Cell::Height(0.0),[](Cell::Height sum, const Cell& cell)
    {
        return sum + cell.height;
    });
}

std::ostream& operator<<(std::ostream& os, const Cell& cell);

}