#include "Game/Cell.h"
#include <numeric> 

namespace Game 
{
static const float earthGravity = 9.80665f;          // m/sec, TODO needs to come or match with Physics.gravity 

Cell::Pressure StaticPressure(float density, float depth, float gravity=earthGravity)
{
    return density * depth * gravity;
}

Cell::Pressure AdjustPressure(const Material& material, Cell::Pressure desiredPressure, float depth, float temperature)
{
    if (material.IsSolid(temperature, desiredPressure))
        return desiredPressure; 
    else 
    if (material.IsGas(temperature, desiredPressure))
        return desiredPressure - StaticPressure(material.Density(temperature, desiredPressure), depth * 0.5);
    else 
        return desiredPressure + StaticPressure(material.Density(temperature, desiredPressure), depth * 0.5);
}

Cell::Cell(const Material& material, Height height, Temperature temperature, Pressure pressure) :
    material(material),
    height(height),
    temperature(temperature),
    pressure(AdjustPressure(material, pressure, static_cast<float>(height), static_cast<float>(temperature)))
{
}

bool Cell::operator==(const Cell& rhs) const
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

float Cell::Volume() const
{
    return static_cast<float>(height);   // horizontal surface is 1m2
}

float Cell::Viscosity() const
{
    // TODO: assumes all materials are newtonian. Lava is not 
    // Lava is also more temperature dependent than the rest. (a factor 100 over 200 Kelvin)
    return material.get().Viscosity(static_cast<float>(temperature), static_cast<float>(pressure));
} 

float Cell::Mass() const
{
    return Volume() * Density();
}

float Cell::Density() const
{
    return material.get().Density(static_cast<float>(temperature), static_cast<float>(pressure));
}

bool Cell::IsGas() const
{
    return material.get().IsGas(static_cast<float>(temperature), static_cast<float>(pressure));
}

bool Cell::IsCompressible() const 
{
    return IsGas() || !material.get().solidDensity; 
}

bool Cell::IsLiquid() const
{
    return material.get().IsLiquid(static_cast<float>(temperature), static_cast<float>(pressure));
}

bool Cell::IsSolid() const
{
    return material.get().IsSolid(static_cast<float>(temperature), static_cast<float>(pressure));
}

bool Cell::IsOpaque() const
{
    return material.get().color.IsOpaque();
}

bool Cell::IsTranslucent() const
{
    return !material.get().color.IsOpaque();
}


void Cell::AddFlow(Orientation dir, Flow df)
{
    if (dir.IsNegative())
    {
        df=-df;
        dir=dir.Axis();
    }
    unsigned index = dir.Index()-1;
    if (df>0.0 && std::numeric_limits<Flow>::max() - df > flow[index])
    {   // TODO check negative as well 
        ASSERT(false);  // flow overflow hurhur 
        flow[index] = std::numeric_limits<Flow>::max();
    }
    else if (df<0.0 && std::numeric_limits<Flow>::min() - df > flow[index])
    {
        ASSERT(false); // flow underflow 
        flow[index] = std::numeric_limits<Flow>::max();
    }
    else 
    {
        flow[dir.Index()-1] += df;
    }
}


void Cell::Heat(float degrees)
{
    float density = Density();
    if (degrees>0)
        if (std::numeric_limits<Temperature>::max() - degrees > temperature)
            temperature += Cell::Temperature(degrees);
        else
            temperature = std::numeric_limits<Temperature>::max();
    if (degrees<0)
        if (temperature>degrees)
            temperature -= Cell::Temperature(-degrees);
        else
            temperature = 0.0;
    pressure = material.get().Pressure(static_cast<float>(temperature), density);
}

Cell::Flow Cell::GetFlow(Orientation dir) const
{
    if (dir.IsNegative())
        return -flow[dir.Axis().Index()-1];
    else 
        return flow[dir.Index()-1];
}

float Cell::GetPressure(Cell::Height atHeight) const
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

bool Cell::TryMerge(const Cell& rhs) 
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


std::ostream& operator<<(std::ostream& os, const Cell& cell)
{
    os << cell.material.get().name << " " << int(static_cast<float>(cell.height)*1000.0f) << "L@" << cell.temperature << "K";
    return os;
}


}
