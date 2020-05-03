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
        // double combust;
        // texture
        double normalDensity;   // g/L at stp  0 °C, ~1000 mbar
        double molarMass;       // G/mol
        double viscosity;       // mu, = N s /m2 = Pa*s t room temperature (25 C) 
        double conductivity;    // J/Mol-K 
        double heatCapacity;    // J/g*K    https://en.wikipedia.org/wiki/Table_of_specific_heat_capacities
        double surfaceTension;  // N/meter hardness when liquid https://en.wikipedia.org/wiki/Surface_tension#Data_table
        double youngsModulus;   // hardness when solid      (Pa) https://en.wikipedia.org/wiki/Young%27s_modulus#Approximate_values
        double opacity;         // mass attenuation coefficient of visible light (400-700nm) absorbed per voxel at 1.0 density
                                // ulatraviolet (sun light) would be 10-400nm 
                                // electric conductivity?


        double Density(double pressure, double temperature) const;

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

    // Evaluate
    double Volume() const;
    double Mass(const Material& material) const;
    double Temperature(const Material& material) const;
    double Mass() const;
    unsigned WindForce() const; // Beaufort

    // Map
    Square At(const Position& p) const override;
    unsigned Latitude() const override;
    unsigned Longitude() const override;
    void Render() const override;
    void Tick(double seconds) override;
protected:
    void RenderPretty() const;
    void RenderAnalysis() const;

    struct Voxel
    {
        bool Solid() const;
        bool Gas() const;
        Square Square(unsigned height) const;
        double Density() const;
        double Mass() const;
        double Pressure() const;
        double Volume() const;
        double Molecules() const;
        double Viscosity() const;
        double Hardness() const;
        double DiffusionCoefficient(const Voxel& to) const;
        
        double Translucency() const;    // I(x) = I0 * Translucency() 
        bool Opaque() const;
        bool Transparent() const;
        Engine::RGBA Color() const;
        void Render(const Position& p, const Directions& visibility, bool analysis) const;

        const Material& material;
        const float temperature;      // Kelvin
        const float density;          // gram/Liters
        //const Engine::Vector flow;    // meter/second
        const Position position;
        const Directions boundary;
    };

    Directions Visibility(const Position& p) const;
    void ComputeForces(double seconds);
    void Diffuse(double seconds);
    Position MaxFlow() const;
    double Mass(class Directions directions) const;
    void Advection(double seconds);
    static Directions IsBoundary(const Position& p, const Position& limit);
    float AtmosphericTemperature(int z) const;
    float AtmosphericPressure(int z) const;
protected:
    class Data
    {
    public:
        Data();
        Data(unsigned longitude, unsigned latitude, unsigned altitude);
        Data(const Data&) = default;
        bool IsInside(const Position& p) const;
        Voxel operator[](const Position& p) const;
        void Set(const Position& location, const Material& material, double temperature, double pressure);

        unsigned Latitude() const;
        unsigned Longitude() const;
        unsigned Altitude() const;
        Directions IsBoundary(const Position& p) const;

        class iterator
        {
        public:
            iterator(const Data& data, const Position& position);
            iterator& operator++();
            //iterator operator++(int);
            bool operator==(const iterator& other) const;
            bool operator!=(const iterator& other) const;
            using value_type = const Voxel;
            value_type operator*() const;

            // iterator traits
            using difference_type = Position;
            using pointer = const Voxel*;
            using reference = const Voxel&&;
            using iterator_category = std::forward_iterator_tag;
            
            const Data& data;
            Position position;
        };
        using value_type = iterator::value_type;
        
        iterator begin() const { return iterator(*this, Position(0, 0, 0)); }
        iterator end() const { return iterator(*this, Position(longitude, 0, 0));  }
    protected:
        Position Stride() const;
        unsigned Index(const Position& p) const;
    private:
        std::vector<const Material*> material;
        std::vector<float> p;           // density
        std::vector<float> u, v, w;     // flux
        std::vector<float> t;           // temperature
        unsigned longitude, latitude, altitude;
    };
    Data voxels;
    double time;
    double planetRadius;    // m
    double atmosphereRadius; // m
    double gravity;         // m/s^2
    float atmosphericTemperature;   // K at 0
};

constexpr double PascalPerAtmosphere = 101325.0;        // Pa/Atm
constexpr double IdealGasConstant = 8.31446261815324e3; // ideal gas constant in L * Pa / K * mol

std::wistream& operator>>(std::wistream& s, VoxelMap& map);


}

