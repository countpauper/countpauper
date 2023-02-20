#include "stdafx.h"
#include "Engine/Maths.h"
#include "Physics/PackedVoxel.h"

namespace Physics
{

    PackedVoxel::PackedVoxel() :
        material(0),
        amount(0),
        temperature(0)
//        flow(Engine::Vector::zero)
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
        return mat->Color(Temperature(), Density());
    }
}

double PackedVoxel::Temperature() const
{
    return static_cast<double>(temperature);
}

void PackedVoxel::SetTemperature(double t)
{
    temperature = static_cast<float>(t);
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

bool PackedVoxel::IsGranular() const
{
    const auto mat = GetMaterial();
    return mat->Granular();
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

double PackedVoxel::Measure(const Material* material) const
{
    if (GetMaterial() == material)
        if (material->normalDensity == 0)
            return 1.0;
        else
            return static_cast<double>(Amount()) / normalAmount;
    else
        return 0;
}

int PackedVoxel::Amount() const
{
    return amount;
}

}