#include "Game/Layer.h"
#include <numeric> 

namespace Game 
{
bool Layer::operator==(const Layer& rhs) const
{
    if (material.get() != rhs.material.get())
        return false;
    if (height != rhs.height)
        return false;
    if (temperature != rhs.temperature)
        return false;
    return true;
}

float Layer::Volume() const
{
    return static_cast<float>(height) * 100.0f;
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

bool Layer::IsOpaque() const
{
    return material.get().color.IsOpaque();
}

bool Layer::IsTranslucent() const
{
    return !material.get().color.IsOpaque();
}


bool Layer::TryMerge(const Layer& rhs) 
{
    if (material.get() != rhs.material.get())
        return false;
    if (this->temperature != rhs.temperature) 
        return false;   // Maybe is close enough (like less than 1 celsius) still mix 
    height += rhs.height;
    return true;
}


std::ostream& operator<<(std::ostream& os, const Layer& layer)
{
    os << layer.material.get().name << " " << int(static_cast<float>(layer.height)*1000.0f) << "L@" << layer.temperature << "K";
    return os;
}


}
