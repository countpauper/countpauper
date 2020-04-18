#pragma once
#include "Map.h"
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
        double melt;    // kelvin
        double boil;    // kelvin
        //double combust;
        // texture
        double normalDensity; // g/L at stp  0 °C, ~1000 mbar
        double conductivity; // J/G-K = J/Mol-K / G/mol
        double transparency; // fraction of vertical light absorbed per voxel
        // electric conductivity?

        static Material vacuum;
        static Material air;
        static Material soil;
        static Material stone;
        static Material water;
    };

    // Generate
    void Space(unsigned x, unsigned y, unsigned z); // Day 0 there is nothing
    void Air(double temperature, double pressure);  // Day 1 the sky
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
private:
    struct Voxel
    {
        Voxel();

        Material& material;
        double temperature;
        double pressure;
        Engine::Vector flow;
        bool fixed;
    };
    std::vector<Voxel> voxels;
    unsigned x, y, z;

    double airTemperature;
    double airPressure;
    double gravity;
};

}

