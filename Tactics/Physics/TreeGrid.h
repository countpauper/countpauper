#pragma once
#include <string>
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
    struct Node
    {
        // material: 0 = vacuum, air, water, earth, stone
        // amount 0 = empty, 15 = full/max density or granularity
        // hot = 0: temperature = 2 * k (0-510), 1 = 510 + 30 k (510-1275) 
        unsigned short material : 3, amount : 4, hot : 1, temperature : 8;

        const double ColdTGradient = 2;
        const double HotTGradient = 30;
        const double HotOffset = 255.0*ColdTGradient;
        static const Material* mats[];

        Node();
        void Set(const Material* material);
        const Material* GetMaterial() const;
        double Temperature() const;
        void SetTemperature(double t);
        double Density() const;
    };
    const Node& operator[](const Position& p) const;
    Node& operator[](const Position& p);
private:

    Box Bounds() const;
    bool Inside(const Position& p) const;
    const Node* Get(const Position& p) const;
    unsigned Index(const Position&p) const;
    using iterator = BoxIterator<TreeGrid, Node>;
    using const_iterator = BoxIterator<const TreeGrid, const Node>;

    Grid grid;
    Size size;
    std::vector<Node> data;
};

}
