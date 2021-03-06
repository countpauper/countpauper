#pragma once
#include "Map.h"
#include "Engine/Vector.h"
#include "Engine/Line.h"
#include "Physics/Plane.h"
#include "Physics/Box.h"
#include "Physics/IEnvironment.h"
#include <string>

namespace Engine { class Mesh;  }
namespace Game
{

class VoxelMap : public Map
{
public:
    VoxelMap();
    VoxelMap(unsigned longitude, unsigned latitude, unsigned altitude);
    ~VoxelMap();

    // Map
    Element Get(const Position& p) const override;
    Physics::Box Bounds() const override;

     // Generate
    void Space(const Engine::Vector& meters); // Day 0 there is nothing
    void Space(const Physics::Size& size); // Day 0 there is nothing
    void World(double radius);
    void Air(double temperature, double meters);     // Day 1 the sky
    void Wind(const Engine::Vector& speed);     // direction in meter/second
    void Sea(double level, double temperature);      // Day 2 Separate the water from the sky 
    void Hill(const Engine::Line& ridgeLine, double stddev);
    void Wall(const Engine::Line& bottomLine, double height, double thickness);
/*    // Evaluate
    double Volume() const;
    double Mass(const Physics::Material& material) const;
    double Temperature(const Physics::Material& material) const;
    double Mass() const;
    unsigned WindForce() const; // Beaufort
*/
    // Map
    void Render() const override;
    void Tick(double seconds) override;
protected:
    void GenerateMesh();
    void RenderPretty() const;
    void RenderAnalysis() const;
    void SetDensityToMeshColor();

    Engine::Coordinate Center(const Position& p) const;
    Physics::Directions Visibility(const Position& p) const;
    float AtmosphericTemperature(double elevation) const;
    float AtmosphericPressure(double elevation) const;
protected:
    Engine::Vector grid;
    Physics::Size size;
    std::unique_ptr<Physics::IEnvironment> physical;
    std::unique_ptr<Engine::Mesh> mesh;
    double time;
    double planetRadius;    // m
    double atmosphereRadius; // m
    double gravity;         // m/s^2
    float atmosphericTemperature;   // K at 0
    Engine::Vector wind;
};

std::wistream& operator>>(std::wistream& s, VoxelMap& map);

}

