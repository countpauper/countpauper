#pragma once
#include <string>
#include <array>
#include <map>
#include "IEnvironment.h"
#include "Engine/Vector.h"
#include "Physics/Size.h"
#include "Physics/Grid.h"
#include "Physics/BoxIterator.h"

namespace Physics
{
class TreeGrid : public IEnvironment
{
public:
    // size in meter, grid in meter/voxel
    TreeGrid(const Engine::Vector& extent, const Grid& grid = Grid());

    size_t Fill(const Engine::IVolume& v, const Material& m, double temperature) override;
    void ApplyForce(const Engine::IVolume& c, const Engine::Vector& v) override;
    void ApplyForce(const Engine::Coordinate& c, double force) override;
    void Heat(const Engine::Coordinate& c, double energy) override;

    void ConnectChange(ChangeSignal::slot_type slot) override;
    double Density(const Engine::IVolume& c) const override;
    double Temperature(const Engine::IVolume& c) const override;
    Engine::Vector Force(const Engine::IVolume& c) const override;
    const Material* GetMaterial(const Engine::Coordinate& c) const override;
    Engine::RGBA Color(const Engine::Line& l) const override;
    void Tick(double seconds) override;
    double Measure(const Material* material) const override;
    std::wstring Statistics() const override;

    class Leaf;
    class Node
    {
    public:
        virtual const Leaf* Get(const Position& p) const = 0;
        static Grid grid;
    };

#pragma pack(push)
#pragma pack(2)
    // TODO: this can be optimized much more by not using polymorphism 
    // The whole tree should be branches. Instead of an octree it could be binary tree (only 2 8-byte pointers)
    // each branch can have a small header if it's a leaf. If so the pointers are not pointers (union?) but material data 
    // perhaps it can point to 2d/3rd material data array instead for nodes that are smaller than a certain treshold, in a memory manager/allocater 
    // or perhaps thise size is 16 bytes / 2 byte per voxel = 8 grids or 64 bytes (8 ptr)
    // Instead of the 24 byte size it can store the x/y/z axis to divide (2 bits, part of branch header) and pass the 3d size on the stack when recursing 
    // anyway it's all even more premature optimization for now. 

    // ... Because I already did the math wrong and a 100x100x100 map with 4 bytes packed data is only 4mb not 4 gb
    class Leaf : public Node
    {
    public:
        Leaf();
        Leaf(const Material& m, double temperature);
        const Leaf* Get(const Position& p) const override;

        void Set(const Material* newMat);
        const Material* GetMaterial() const;
        Engine::RGBA Color() const;
        double Temperature() const;
        void SetTemperature(double t);
        double Density() const;
    private:
        // material: 0 = vacuum, air, water, earth, stone
        // amount 0 = empty, 15 = full/max density or granularity
        // hot = 0: temperature = 2 * k (0-510), 1 = 510 + 30 k (510-1275) 
        uint8_t material : 3, amount : 4, hot : 1, temperature : 8;

        static const int ColdTGradient = 2;
        static const int HotTGradient = 30;
        static const int HotOffset = 255*ColdTGradient;
        static const Material* mats[];
    };
#pragma pack(pop)
private:

    class Branch : public Node
    {
    public:
        Branch(const Size& size);
        unsigned Fill(const Engine::IVolume& v, const Position& offset, const Material& m, double temperaturee, const Material* restMaterial=nullptr, double restTemperature = 0);
        std::pair<double, double> GetTemperature(const Engine::IVolume& v, const Position& ofset) const;
        const Leaf* Get(const Position& p) const;
        double Measure(const Material* material) const;
        Size GetSize() const;
        void GetStatistics(std::map<unsigned, unsigned>& leaves, unsigned& branches) const;
    private:
        Position Pivot() const;
        unsigned GetIndex(const Position& p) const;
        Position GetOffset(unsigned index) const;
        Box GetBounds(unsigned index) const;
        static bool AllIn(const Box& box, const Engine::IVolume& v);
        static unsigned  Overlap(const Box& box, const Engine::IVolume& v);
        Size size;
        std::array<std::unique_ptr<Node>, 8> nodes;
    };
    const Leaf* TreeGrid::Get(const Position& p) const;
    std::unique_ptr<Branch> root;
};

}
