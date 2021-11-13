#pragma once
#include <string>
#include <array>
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
    TreeGrid(const Engine::Vector& extent, const Engine::Vector& grid = Engine::Vector(1, 1, 1));

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
    
    class Leaf;
    class Node
    {
    public:
        virtual const Leaf* Get(const Position& p) const = 0;
        static Grid grid;
    };

    class Leaf : public Node
    {
    public:
        Leaf();
        Leaf(const Material& m, double temperature);
        const Leaf* Get(const Position& p) const override;

        void Set(const Material* newMat);
        const Material* GetMaterial() const;
        double Temperature() const;
        void SetTemperature(double t);
        double Density() const;
    private:
        // material: 0 = vacuum, air, water, earth, stone
        // amount 0 = empty, 15 = full/max density or granularity
        // hot = 0: temperature = 2 * k (0-510), 1 = 510 + 30 k (510-1275) 
        unsigned short material : 3, amount : 4, hot : 1, temperature : 8;

        const int ColdTGradient = 2;
        const int HotTGradient = 30;
        const int HotOffset = 255*ColdTGradient;
        static const Material* mats[];
    };
private:

    class Branch : public Node
    {
    public:
        Branch(const Size& size);
        unsigned Fill(const Engine::IVolume& v, const Position& offset, const Material& m, double temperature);
        std::pair<double, double> GetTemperature(const Engine::IVolume& v, const Position& ofset) const;
        const Leaf* Get(const Position& p) const;
    private:
        Position Pivot() const;
        unsigned GetIndex(const Position& p) const;
        Position GetOffset(unsigned index) const;
        Box GetBounds(unsigned index) const;
        static bool AllIn(const Box& box, const Engine::IVolume& v);
        static double Overlap(const Box& box, const Engine::IVolume& v);
        Size size;
        std::array<std::unique_ptr<Node>, 8> nodes;
    };
    const Leaf* TreeGrid::Get(const Position& p) const;
    std::unique_ptr<Branch> root;
};

}
