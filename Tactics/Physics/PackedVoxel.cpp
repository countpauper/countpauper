#include "stdafx.h"
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
            translucency = 0.2 + (0.5 * amount / maxAmount);
        else if (mat->Gas(temperature))
            translucency = 0.0;
        return mat->color.Translucent(translucency);
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
    int it = int(std::floor(t));
    if (t > HotOffset)
    {
        hot = 1;
        temperature = (it - HotOffset) / HotTGradient;
    }
    else
    {
        hot = 0;
        temperature = it / ColdTGradient;
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
    return mat->Gas(Temperature());
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

int PackedVoxel::Amount() const
{
    return amount;
}

}