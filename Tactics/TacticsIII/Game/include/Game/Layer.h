#pragma once 

#include "Utility/FixedPoint.h"
#include "Game/Material.h"

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
    bool TryMerge(const Layer& rhs);
};

std::ostream& operator<<(std::ostream& os, const Layer& layer);

}