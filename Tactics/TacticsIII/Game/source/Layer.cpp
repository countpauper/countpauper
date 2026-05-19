#include "Game/Layer.h"
#include <numeric> 

namespace Game 
{
static const float earthGravity = 9.80665f;          // m/sec, TODO needs to come or match with Physics.gravity 

float StaticPressure(float density, float depth, float gravity=earthGravity)
{
    return density * depth * gravity;
}

float AdjustPressureLevel(const Material& material, float temperature, float desiredPressure, float height)
{
    if (material.IsSolid(temperature))
        return desiredPressure; 
    else if (material.IsLiquid(temperature))
        return desiredPressure + StaticPressure(material.liquidDensity, height/2);
    else 
        return desiredPressure - StaticPressure(material.Density(temperature, desiredPressure), height/2);
}

Layer::Layer(const Material& material, Height height, Temperature temperature, float pressure) :
    material(material),
    height(height),
    temperature(temperature),
    density(material.Density(static_cast<float>(this->temperature), 
        AdjustPressureLevel(material, static_cast<float>(temperature), pressure, static_cast<float>(height))))
{
}

bool Layer::operator==(const Layer& rhs) const
{
    if (material.get() != rhs.material.get())
        return false;
    if (height != rhs.height)
        return false;
    if (temperature != rhs.temperature)
        return false;
    if (density != rhs.density)
        return false;
    for(unsigned idx = 0; idx<flow.size(); ++idx)
        if (flow[idx]!=rhs.flow[idx])
            return false;
    return true;
}

float Layer::Volume() const
{
    return static_cast<float>(height);   // horizontal surface is 1m2
}

float Layer::Viscosity() const
{
    // TODO: assumes all materials are newtonian. Lava is not 
    // Lava is also more temperature dependent than the rest. (a factor 100 over 200 Kelvin)
    return material.get().Viscosity(static_cast<float>(temperature));
} 

float Layer::Mass() const
{
    return Volume() * static_cast<float>(density);
}

bool Layer::IsGas() const
{
    return material.get().IsGas(static_cast<float>(temperature));
}

bool Layer::IsCompressible() const 
{
    return IsGas() || !material.get().solidDensity; 
}

bool Layer::IsLiquid() const
{
    return material.get().IsLiquid(static_cast<float>(temperature));
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


void Layer::AddFlow(Orientation dir, Flow df)
{
    if (dir.IsNegative())
    {
        df=-df;
        dir=dir.Axis();
    }
    unsigned index = dir.Index()-1;
    if (df>0.0 && std::numeric_limits<Flow>::max() - df > flow[index])
    {   // TODO check negative as well 
        assert(false);  // flow overflow hurhur 
        flow[index] = std::numeric_limits<Flow>::max();
    }
    else if (df<0.0 && std::numeric_limits<Flow>::min() - df > flow[index])
    {
        assert(false); // flow underflow 
        flow[index] = std::numeric_limits<Flow>::max();
    }
    else 
    {
        flow[dir.Index()-1] += df;
    }
}

Layer::Flow Layer::GetFlow(Orientation dir) const
{
    if (dir.IsNegative())
        return -flow[dir.Axis().Index()-1];
    else 
        return flow[dir.Index()-1];
}

float Layer::GetPressure(ZType atHeight) const
{
    auto averagePressure = material.get().Pressure(static_cast<float>(temperature), static_cast<float>(density));
    return averagePressure + StaticPressure(static_cast<float>(density), static_cast<float>(height)/2 - static_cast<float>(atHeight)); 
}

bool Layer::TryMerge(const Layer& rhs) 
{
    if (material.get() != rhs.material.get())
        return false;
    if (this->temperature != rhs.temperature) 
        return false;   // Maybe is close enough (like less than 1 celsius) still mix
    float totalMass = Mass() + rhs.Mass(); 
    height += rhs.height;
    density = totalMass / Volume();

    // TODO merge flows, mostly by averaging I suppose
    return true;
}


std::ostream& operator<<(std::ostream& os, const Layer& layer)
{
    os << layer.material.get().name << " " << int(static_cast<float>(layer.height)*1000.0f) << "L@" << layer.temperature << "K";
    return os;
}


}
