#include "Physics/Material.h"
#include "Engine/Color.h"
namespace Physics
{
#pragma pack(push)
#pragma pack(2)

    class PackedVoxel
    {
    public:
        PackedVoxel();
        PackedVoxel(const Material& m, double temperature);

        void Set(const Material* newMat, unsigned  amount = 15);
        void Set(const Material& newMat, unsigned  amount = 15) { Set(&newMat, amount); }
        const Material* GetMaterial() const;
        Engine::RGBA Color() const;
        double Temperature() const;
        void SetTemperature(double t);
        double Density() const;
        int Amount() const;
    private:

        // material: 0 = vacuum, air, water, earth, stone
        // amount 0 = empty, 15 = full/max density or granularity
        // hot = 0: temperature = 2 * k (0-510), 1 = 510 + 30 k (510-1275) 
        uint8_t material : 3, amount : 4, hot : 1, temperature : 8;

        static const int ColdTGradient = 2;
        static const int HotTGradient = 30;
        static const int HotOffset = 255 * ColdTGradient;
        static const Material* mats[];
    };
#pragma pack(pop)
}