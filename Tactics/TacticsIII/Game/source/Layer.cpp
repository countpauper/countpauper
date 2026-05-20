#include "Game/Layer.h"
#include <numeric> 

namespace Game 
{
static const float earthGravity = 9.80665f;          // m/sec, TODO needs to come or match with Physics.gravity 

Layer::Pressure StaticPressure(float density, float depth, float gravity=earthGravity)
{
    return density * depth * gravity;
}

Layer::Pressure AdjustPressure(const Material& material, Layer::Pressure desiredPressure, float depth, float temperature)
{
    if (material.IsSolid(temperature, desiredPressure))
        return desiredPressure; 
    else 
    if (material.IsGas(temperature, desiredPressure))
        return desiredPressure - StaticPressure(material.Density(temperature, desiredPressure), depth * 0.5);
    else 
        return desiredPressure + StaticPressure(material.Density(temperature, desiredPressure), depth * 0.5);
}

Layer::Layer(const Material& material, Height height, Temperature temperature, Pressure pressure) :
    material(material),
    height(height),
    temperature(temperature),
    pressure(AdjustPressure(material, pressure, static_cast<float>(height), static_cast<float>(temperature)))
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
    if (pressure != rhs.pressure)
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
    return material.get().Viscosity(static_cast<float>(temperature), static_cast<float>(pressure));
} 

float Layer::Mass() const
{
    return Volume() * Density();
}

float Layer::Density() const
{
    return material.get().Density(static_cast<float>(temperature), static_cast<float>(pressure));
}

bool Layer::IsGas() const
{
    return material.get().IsGas(static_cast<float>(temperature), static_cast<float>(pressure));
}

bool Layer::IsCompressible() const 
{
    return IsGas() || !material.get().solidDensity; 
}

bool Layer::IsLiquid() const
{
    return material.get().IsLiquid(static_cast<float>(temperature), static_cast<float>(pressure));
}

bool Layer::IsSolid() const
{
    return material.get().IsSolid(static_cast<float>(temperature), static_cast<float>(pressure));
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


void Layer::Heat(float degrees)
{
    float density = Density();
    if (degrees>0)
        if (std::numeric_limits<Temperature>::max() - degrees > temperature)
            temperature += Layer::Temperature(degrees);
        else
            temperature = std::numeric_limits<Temperature>::max();
    if (degrees<0)
        if (temperature>degrees)
            temperature -= Layer::Temperature(-degrees);
        else
            temperature = 0.0;
    pressure = material.get().Pressure(static_cast<float>(temperature), density);
}

Layer::Flow Layer::GetFlow(Orientation dir) const
{
    if (dir.IsNegative())
        return -flow[dir.Axis().Index()-1];
    else 
        return flow[dir.Index()-1];
}

float Layer::GetPressure(Layer::Height atHeight) const
{
    const auto& mat = material.get();
    if (atHeight > height)
        throw std::invalid_argument("Height outside of layers");
    
    if (mat.molarmass == 0.0)
        return 0.0;
    if (material.get().IsSolid(static_cast<float>(temperature), static_cast<float>(pressure)))
        return std::numeric_limits<float>::quiet_NaN();
    auto averagePressure = pressure;
    return averagePressure + StaticPressure(Density(), static_cast<float>(height)/2 - static_cast<float>(atHeight)); 
}

bool Layer::TryMerge(const Layer& rhs) 
{
    if (material.get() != rhs.material.get())
        return false;
    if (this->temperature != rhs.temperature) 
        return false;   // Maybe is close enough (like less than 1 celsius) still mix
    pressure = (pressure * Volume() + rhs.pressure * rhs.Volume()) / (Volume() + rhs.Volume());
    height += rhs.height;

    // TODO merge flows, mostly by averaging I suppose
    return true;
}


std::ostream& operator<<(std::ostream& os, const Layer& layer)
{
    os << layer.material.get().name << " " << int(static_cast<float>(layer.height)*1000.0f) << "L@" << layer.temperature << "K";
    return os;
}


}
