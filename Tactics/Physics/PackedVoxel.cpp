#include "stdafx.h"
#include "Engine/Maths.h"
#include "Physics/PackedVoxel.h"

namespace Physics
{

    PackedVoxel::PackedVoxel() :
        material(0),
        amount(0),
        hot(0),
        temperature(0)
    {
    }

    PackedVoxel::PackedVoxel(const Material& m, double temperature, double fraction) :
        amount(0)
    {
        int amt = std::min(maxAmount, int(PackedVoxel::normalAmount * fraction));

        Set(&m, amt);
        SetTemperature(temperature);
    }

    const Material* PackedVoxel::mats[] =
    {
        &Material::air,
        &Material::water,
        &Material::soil,
        &Material::stone,
        nullptr
    };

    void PackedVoxel::Set(const Material* newMat, unsigned amt)
    {
        if (!newMat->normalDensity)
        {
            amt = 0;
            newMat = mats[0];
        }
        for (material = 0; mats[material]; material++)
        {
            if (mats[material] == newMat)
            {
                amount = amt;
                return;
            }
        }
        throw std::runtime_error("Material needs to be a standard one");
    }

    void PackedVoxel::Increase(int delta)
    {
        int newAmount = amount + delta;
        if (newAmount < 1)
            throw std::runtime_error("All material drained");
        else if (newAmount > PackedVoxel::maxAmount)
            throw std::runtime_error("Material overflow");
        amount = newAmount;
    }


const Material* PackedVoxel::GetMaterial() const
{
    if (!amount)
        return &Material::vacuum;
    else
        return mats[material];
}

Engine::RGBA PackedVoxel::Color() const
{
    if (!material)
        return Engine::RGBA();
    else
    {
        const auto* mat = GetMaterial();
        double temperature = Temperature();
        double translucency = 1.0;
        if (mat->Fluid(temperature))
            translucency = 0.2+std::min(0.8, Density()/2500.0);
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
}

double PackedVoxel::Temperature() const
{
    if (hot)
    {
        return HotOffset + temperature * HotTGradient;
    }
    else
    {
        return temperature * ColdTGradient;
    }
}

void PackedVoxel::SetTemperature(double t)
{
    if (t > HotOffset)
    {
        hot = 1;
        temperature = int(std::round((t - HotOffset) / HotTGradient));
    }
    else
    {
        hot = 0;
        temperature = int(std::round(t / ColdTGradient));
    }
}


bool PackedVoxel::IsFluid() const
{
    const auto mat = GetMaterial();
    return mat->Fluid(Temperature());
}

bool PackedVoxel::IsSolid() const
{
    const auto mat = GetMaterial();
    return mat->Solid(Temperature());
}
bool PackedVoxel::IsGas() const
{
    const auto mat = GetMaterial();
    return mat->Gas(Temperature());
}

double PackedVoxel::Density() const
{
    return Amount()/ double(normalAmount) * GetMaterial()->normalDensity;    // TODO: temperature? amount? 
}

double PackedVoxel::Mass(double volume) const
{
    return Density() * volume;
}

int PackedVoxel::Amount() const
{
    return amount;
}

}