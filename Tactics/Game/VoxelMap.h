#pragma once
#include "Map.h"
#include "Engine/Color.h"
#include "Engine/Vector.h"
#include "Plane.h"
#include <string>

namespace Game
{



class VoxelMap : public Map
{
public:
    VoxelMap();

    struct Material
    {
        std::wstring_view name;
        Element element;
        Engine::RGBA color;
        double melt;    // kelvin
        double boil;    // kelvin
        //double combust;
        // texture
        double normalDensity; // g/L at stp  0 °C, ~1000 mbar
        double molarMass;   // G/mol
        double conductivity; // J/Mol-K 
        double surfaceTension;  // N/meter hardness when liquid https://en.wikipedia.org/wiki/Surface_tension#Data_table
        double youngsModulus;   // hardness when solid      (Pa) https://en.wikipedia.org/wiki/Young%27s_modulus#Approximate_values
        double granularity;  // parts per g in solid state, determines strength and maximum humidity 
        double opacity;         // mass attenuation coefficient of visible light (400-700nm) absorbed per voxel at 1.0 density
                                // ulatraviolet (sun light) would be 10-400nm 
                                
                                // electric conductivity?

        static const Material vacuum;
        static const Material air;
        static const Material soil;
        static const Material stone;
        static const Material water;
    };

    // Generate
    void Space(unsigned x, unsigned y, unsigned z); // Day 0 there is nothing
    void World(double radius);
    void Air(double temperature, double meters);     // Day 1 the sky
    void Water(int level, double temperature);      // Day 2 Separate the water from the sky 
    void Hill(Engine::Coordinate p1, Engine::Coordinate p2, float stddev);
    void Compute();     // At the 7th day god rested
    // Map
    Square At(const Position& p) const override;
    void Render() const override;
    void Tick(double seconds) override;
protected:
    unsigned Latitude() const override;
    unsigned Longitude() const override;
    unsigned Altitude() const;
    void RenderPretty() const;
    void RenderAnalysis() const;
    class Directions
    {   
    public:
        Directions();
        explicit Directions(uint16_t flags);

        Directions& operator|=(const Direction& dir);
        bool operator[](const Direction& dir) const;
        bool empty() const;
    private:
        uint16_t flags;
    };

    struct Voxel
    {
        Voxel();
        bool Solid() const;
        bool Gas() const;
        Square Square(unsigned height) const;
        double Density() const;
        double Pressure() const;
        double Volume() const;
        double Molecules() const;
        double Harndess() const;

        double Translucency() const;    // I(x) = I0 * Translucency() 
        bool Opaque() const;
        bool Transparent() const;
        Engine::RGBA Color() const;
        void Render(const Position& p, const Directions& visibility, bool analysis) const;

        const Material* material;
    
        float temperature;      // Kelvin
        float mass;             // gram, mass determines density and pressure
        float humidity;         // Total Water mass (g)
        Engine::Vector flow;    // meter/second
        bool boundary;          // boundary voxel is constant when evolving (might be used for water, air & lava flow in or out)
        bool fixed;
        Directions visibility;
    };

    const Voxel& Get(const Position& p) const;
    Voxel& Get(const Position& p);
    unsigned VoxelIndex(const Position& p) const;
    Position Stride() const;
    bool IsBoundary(const Position& p) const;
    bool IsInside(const Position& p) const;
    Position GetPosition(const Voxel* pVoxel) const;
    void Iterate(Position& p) const;
    Directions Visibility(const Position& p) const;
    double DiffusionRate(const Position& p, const Direction& d) const;
    void ComputeFlow(double seconds);
    void Flow(double seconds);
    void Diffuse(double seconds);
private:
    std::vector<Voxel> voxels;
    unsigned longitude, latitude, altitude;
    double planetRadius;
    double gravity;
};

std::wistream& operator>>(std::wistream& s, VoxelMap& map);

}

