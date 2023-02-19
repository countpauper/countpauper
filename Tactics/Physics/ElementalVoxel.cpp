#include "stdafx.h"
#include "Physics/ElementalVoxel.h"
#include "Engine/Maths.h"

namespace Physics
{

ElementalVoxel::ElementalVoxel() :
    stone(0),
    water(0),
    air(0),
    fire(0),
    nature(0)
    //        flow(Engine::Vector::zero)
{
}

ElementalVoxel::ElementalVoxel(const Material& m, double temperature, double fraction) :
    ElementalVoxel()
{
    int amt = std::min(std::numeric_limits<decltype(stone)>::max(), static_cast<unsigned short>(ElementalVoxel::normalAmount * fraction));

    Set(&m, amt);
    SetTemperature(temperature);
}

const Material* ElementalVoxel::mats[] =
{
    &Material::air,
    &Material::water,
    &Material::soil,
    &Material::stone,
    nullptr
};

void ElementalVoxel::Set(const Material* newMat, unsigned amt)
{
    if (newMat == &Material::air)
    {
        air = amt;
        stone = 0;
        water = 0;
        nature = 0;
    }
    else if (newMat == &Material::water)
    {
        water = amt;
        air = std::max(0, normalAmount - static_cast<int>(water));
        stone = 0;
        nature = 0;
    }
    else if (newMat == &Material::stone)
    {
        stone = normalAmount*2;
        air = std::max(0, normalAmount - static_cast<int>(amt));
        water = 0;
        nature = 0;
    }
    else if (newMat == &Material::soil)
    {
        stone = normalAmount/5;
        water = normalAmount/5;
        nature = amt;
    }
    else
    {
        throw std::runtime_error("Material needs to be a standard one");
    }
}

void ElementalVoxel::Increase(int delta)
{
    throw std::runtime_error("Unimplemented");
    // could GetMaterial and Set(material, amount+delta) where amount depends on the type of material 
}

const Material* ElementalVoxel::GetMaterial() const
{
    if ((water >= (normalAmount - air)) && (water > nature))
    {
        return &Material::water;
    }
    else if ((air < normalAmount/10) && (stone >= normalAmount))
    {
        return &Material::stone;
    }
    else if ((nature > water) && (stone < normalAmount))
    {
        return &Material::soil;
    }
    else if (air > water)
    {
        return &Material::air;
    }
    else if (air + water + stone + nature == 0.0)
    {
        return &Material::vacuum;
    }
    else
    {
        throw std::runtime_error("Unrecognized material");
    }
}

Engine::RGBA ElementalVoxel::Color() const
{
    const auto* mat = GetMaterial();
    double temperature = Temperature();
    double translucency = 1.0;
    if (mat->Fluid(temperature))
        translucency = 0.2 + std::min(0.8, Density() / 2500.0);
    else if (mat->Gas(temperature))
        translucency = 0.0;
    auto baseColor = mat->color.Translucent(translucency);

    if (temperature < 270)
        return Engine::Lerp(baseColor, Engine::RGBA(0, 0, 255), (temperature - 270) / 270.0);
    else if (temperature > 750)
        return Engine::Lerp(baseColor, Engine::RGBA(255, 64, 0), (temperature - 750) / (mat->boil - 700));
    else
        return baseColor;
}

double ElementalVoxel::Temperature() const
{
    return static_cast<double>(fire)/10.0;
}

void ElementalVoxel::SetTemperature(double t)
{
    fire = static_cast<uint16_t>(std::round(t * 10.0));
}

bool ElementalVoxel::IsFluid() const
{
    const auto mat = GetMaterial();
    return mat->Fluid(Temperature());
}

bool ElementalVoxel::IsSolid() const
{
    const auto mat = GetMaterial();
    return mat->Solid(Temperature());
}
bool ElementalVoxel::IsGas() const
{
    const auto mat = GetMaterial();
    return mat->Gas(Temperature());
}

bool ElementalVoxel::IsGranular() const
{
    return ((stone > 0) && (stone < normalAmount));
}

double ElementalVoxel::Density() const
{
    double t = Temperature();
    assert(!Material::stone.Gas(t));    // assume no gas pressure from stone
    constexpr double volume = 10 * 10 * 3333; // L
    // TODO: no temperature density is used 
    double stoneWeight = static_cast<double>(stone) / normalAmount * volume * Material::stone.normalDensity;
    double waterWeight = static_cast<double>(water) / normalAmount * volume * Material::water.normalDensity;  // weight of water in g
    double airWeight = static_cast<double>(air) / normalAmount * volume * Material::air.normalDensity;  // weight of air in g   
    double natureWeight = static_cast<double>(nature) / normalAmount * volume * Material::water.normalDensity; // nature is made of water 
    return (stoneWeight + waterWeight + airWeight + natureWeight) / volume;
}

double ElementalVoxel::Measure(const Material* m) const
{
    if (m == &Material::stone)
    {
        if (stone >= normalAmount)
            return static_cast<double>(normalAmount - water - air) / normalAmount;
        else
            return 0.0;
    }
    else if (m == &Material::water)
    {
        return static_cast<double>(water) / normalAmount;
    }
    else if (m == &Material::air)
    {
        return static_cast<double>(air) / normalAmount;
    }
    else if (m == &Material::soil)
    {
        if (stone < normalAmount)
            return static_cast<double>(normalAmount - air) / normalAmount;
        else
            return 0.0;
    }
    else if (m == &Material::vacuum)
    {
        if (stone == 0)
            return static_cast<double>(normalAmount - water - air) / normalAmount;
        else
            return 0;
    }
    else
    {
        throw std::runtime_error("Unmeasurable material " + std::string(m->name));
    }
}

bool ElementalVoxel::PropagateFire(ElementalVoxel& neighbour)
{
    auto dFire = static_cast<int>(fire) - neighbour.fire;
    if (dFire >= 2)
    {
        fire -= 1;
        neighbour.fire += 1;
        return true;
    }
    else if (dFire <= -2)
    {
        fire += 1;
        neighbour.fire -= 1;
        return true;
    }
    else
        return false;
}

}