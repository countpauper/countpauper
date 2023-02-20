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
    double density = 0;
    double soilPercentage = Measure(&Material::soil);
    if (soilPercentage > 0)
    {   // soil is made up of sand, water and nature in a mix. don't measure stone and water
        density += soilPercentage * Material::soil.Density(PascalPerAtmosphere, t);
    }
    else
    {
        double stonePercentage = Measure(&Material::stone);
        density += stonePercentage * Material::stone.Density(PascalPerAtmosphere, t);
        
        double waterFraction = Measure(&Material::water);
        if (waterFraction < 1.0)  // water level
            density += Measure(&Material::water)  * Material::water.Density(PascalPerAtmosphere, t);
        else     // high pressure water
            density += Material::water.Density(waterFraction * PascalPerAtmosphere, t);
    }
    double airFraction = Measure(&Material::air);
    if (airFraction <= 1)
        density += airFraction * Material::air.Density(PascalPerAtmosphere, t);
    else
        density += Material::air.Density(airFraction * PascalPerAtmosphere, t);
    return density;
}

double ElementalVoxel::Measure(const Material* m) const
{
    assert(nature == 0);     // nature is not taken into account. If could be the amount of soil but it's more plant mass on top of it 
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