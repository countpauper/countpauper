#pragma once 

#include "Utility/FixedPoint.h"
#include "Game/Material.h"
#include "Utility/Concepts.h"
#include <functional>
#include <numeric>

namespace Game
{




struct Layer 
{
    using Amount = Engine::FixedPoint<8, uint16_t>;   
    using Temperature = Engine::FixedPoint<3,uint16_t>;        

    std::reference_wrapper<const Material> material;
    Amount amount; 
    Temperature temperature; 
    
    bool operator==(const Layer& rhs) const;
    float Volume() const;
    float Density() const;  
    bool IsGas() const;
    bool IsSolid() const;
    bool IsOpaque() const;
    bool IsTranslucent() const;

    using Predicate = std::function<bool(const Layer&)>;
    bool TryMerge(const Layer& rhs);
};

template<typename R>
concept RangeOfLayer = RangeOf<R, Layer>;

//Layer::Amount SumAmount(RangeOfLayer auto&& range);
Layer::Amount SumAmount(RangeOfLayer auto&& range)
{
    return std::accumulate(std::ranges::begin(range), std::ranges::end(range), Layer::Amount(0.0),[](Layer::Amount sum, const Layer& layer)
    {
        return sum + layer.amount;
    });
}

std::ostream& operator<<(std::ostream& os, const Layer& layer);

}