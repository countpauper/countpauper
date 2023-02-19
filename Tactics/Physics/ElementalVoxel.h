#include "Physics/Material.h"
#include "Engine/Color.h"

namespace Physics
{

class ElementalVoxel
{
public:
    static const int normalAmount = 10000;

    ElementalVoxel();
    // TODO: amount could be double density on interface as well, but it's much slower to fill large areas and compute over an over again
    // and the discrete physics is not that continuous either
    ElementalVoxel(const Material& m, double temperature, double fraction = 1.0);

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
    bool IsGranular() const;
    double Density() const;
    double Measure(const Material* m) const;
    bool PropagateFire(ElementalVoxel& neighbour);
    //  double Mass(double volume) const;
    // int Amount() const; // for gas: pressure, for fluid: level, for solid: granularity  

    uint16_t air, water, stone, fire, nature;
private:
    //Engine::Vector flow;    // TODO: optimize size: can be in (current? normal? density of the material and then use realistic speeds from cm/s to high wind speed ~40m/sec using ~16 bit shorts)
    static const Material* mats[];
};

}