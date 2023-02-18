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
    int amt = std::min(255, int(ElementalVoxel::normalAmount * fraction));

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
        stone = 200;
        air = std::max(0, normalAmount - static_cast<int>(amt));
        water = 0;
        nature = 0;
    }
    else if (newMat == &Material::soil)
    {
        stone = 20;
        water = 20;
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
    else if ((air < 10) && (stone >= normalAmount))
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

constexpr double coefficients[4] = { 0.00036, -0.0862, 8.7063, 0 };

double ElementalVoxel::Temperature() const
{
    return pow(fire, 3) * coefficients[0] +
        pow(fire, 2) * coefficients[1] +
        fire * coefficients[2] +
        coefficients[3];
}

void ElementalVoxel::SetTemperature(double t)
{
    // simplified Cardano's method, knowing there's one solution with these coefficients
    constexpr double a = coefficients[0];
    constexpr double b = coefficients[1];
    constexpr double c = coefficients[2];
    double d = (coefficients[3] - t);

    // define a depressed cubic t^3 +pt +q by substitution t=x+b/3a 
    constexpr double p = (3 * a*c - b * b) / (9 * a*a);
    double q = (9 * a*b*c - 27 * a*a*d - 2 * b*b*b) / (54 * a*a*a);

    // solve the roots of the u and v of the depressed cubic
    double D = std::pow(p, 3) + pow(q, 2);
    double sqrtD = sqrt(D);
    double u = std::cbrt(q - sqrtD);
    double v = std::cbrt(q + sqrtD);

    // convert back to the first root of the original cubic equation. the other cubes are complex and ignored
    double root = u + v - b / (3 * a);
    // assign the rounded root to the fire as that inverts the polynomial
    fire = static_cast<uint8_t>(std::max(0.0, std::min(255.0, std::round(root))));
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

}