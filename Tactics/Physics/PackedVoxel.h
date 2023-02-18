#pragma once
#include "Physics/Material.h"
#include "Engine/Color.h"
#include "Engine/Vector.h"

namespace Physics
{
#pragma pack(push)
#pragma pack(2)

    class PackedVoxel
    {
    public:
        static const int maxAmount = 15;
        static const int normalAmount = 10;
        PackedVoxel();
        // TODO: amount could be double density on interface as well, but it's much slower to fill large areas and compute over an over again
        // and the discrete physics is not that continuous either
        PackedVoxel(const Material& m, double temperature, double fraction=1.0);

        void Set(const Material* newMat, unsigned  amount = normalAmount);
        void Set(const Material& newMat, unsigned  amount = normalAmount) { Set(&newMat, amount); }
        void Increase(int delta);
        const Material* GetMaterial() const;
        Engine::RGBA Color() const;
        double Temperature() const;
        void SetTemperature(double t);
        bool IsFluid() const;
        bool IsSolid() const;
        bool IsGas() const;
        double Density() const;
        double Mass(double volume) const;
        double Measure(const Material* material) const;
        int Amount() const; // for gas: pressure, for fluid: level, for solid: granularity  
    private:

        // material: 0 = vacuum, air, water, earth, stone
        // amount 0 = empty, 10 = full, 15 = max density or granularity
        uint8_t material : 2, amount : 4;
        float temperature;
        //Engine::Vector flow;    // TODO: optimize size: can be in (current? normal? density of the material and then use realistic speeds from cm/s to high wind speed ~40m/sec using ~16 bit shorts)
        static const Material* mats[];
    };
#pragma pack(pop)
}