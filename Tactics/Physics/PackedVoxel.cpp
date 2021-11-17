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

    PackedVoxel::PackedVoxel(const Material& m, double temperature) :
        amount(0)
    {
        Set(&m);
        SetTemperature(temperature);
    }

    const Material* PackedVoxel::mats[] =
    {
        &Material::vacuum,
        &Material::air,
        &Material::water,
        &Material::soil,
        &Material::stone,
        nullptr
    };

    void PackedVoxel::Set(const Material* newMat)
    {
        for (material = 0; mats[material]; material++)
        {
            if (mats[material] == newMat)
            {
                if (newMat->normalDensity)
                    amount = 15;
                else
                    amount = 0;
                return;
            }
        }
        throw std::runtime_error("Material needs to be a standard one");
    }


const Material* PackedVoxel::GetMaterial() const
{
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
            translucency = 0.5;
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

double PackedVoxel::Density() const
{
    return GetMaterial()->normalDensity;    // TODO: temperature? 
}


}