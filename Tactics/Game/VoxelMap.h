#pragma once
#include "Map.h"
#include "Engine/Color.h"
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
        double conductivity; // J/G-K = J/Mol-K / G/mol
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
    void Air(double temperature);                   // Day 1 the sky
    void Rock(int min, double stddev, double gravity);                      // Day 3 dry land and plants
    void Sun(double time);                          // Day 4 Sun, Moon and stars
    void Water();                                   // Day 5 Animals and sea life
    void Populate();                                // Day 6 Land animals and people

    // Map
    Square At(const Position& p) const override;
    void Render() const override;
protected:
    unsigned Latitude() const override;
    unsigned Longitude() const override;
    unsigned Altitude() const;
    struct Voxel
    {
        Voxel();
        bool Solid() const;
        bool Gas() const;
        Square Square() const;
        double Density() const;

        double Translucency() const;    // I(x) = I0 * Translucency() 
        bool Opaque() const;
        bool Transparent() const;
        Engine::RGBA Color() const;

        const Material* material;
        double temperature;     // Kelvin
        double mass;            // gram, mass determines density and pressure
        Engine::Vector flow;    // meter/second
        bool fixed;
    };
    const Voxel& Get(unsigned x, unsigned y, unsigned z) const;
    Voxel& Get(unsigned x, unsigned y, unsigned z);
private:

    std::vector<Voxel> voxels;
    unsigned x, y, z;

    double airTemperature;
    double gravity;
};

std::wistream& operator>>(std::wistream& s, VoxelMap& map);

}

