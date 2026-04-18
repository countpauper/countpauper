#include "Game/Layer.h"

namespace Game 
{
bool Layer::operator==(const Layer& rhs) const
{
    if (material.get() != rhs.material.get())
        return false;
    if (amount != rhs.amount)
        return false;
    if (temperature != rhs.temperature)
        return false;
    return true;
}

float Layer::Volume() const
{
    return static_cast<float>(amount) * 100.0f;
}

float Layer::Density() const
{
    return material.get().Density(static_cast<float>(temperature));
}

 
bool Layer::IsGas() const
{
    return material.get().IsGas(static_cast<float>(temperature));
}

bool Layer::IsSolid() const
{
    return material.get().IsSolid(static_cast<float>(temperature));
}

bool Layer::TryMerge(const Layer& rhs) 
{
    if (material.get() != rhs.material.get())
        return false;
    if (this->temperature != rhs.temperature) 
        return false;   // Maybe is close enough (like less than 1 celsius) still mix 
    amount += rhs.amount;
    return true;
}


std::ostream& operator<<(std::ostream& os, const Layer& layer)
{
    os << layer.material.get().name << " " << int(static_cast<float>(layer.amount)*1000.0f) << "L@" << layer.temperature << "K";
    return os;
}

}