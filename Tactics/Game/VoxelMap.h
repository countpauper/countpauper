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
    unsigned Latitude() const override;
    unsigned Longitude() const override;
    void Render() const override;
    void Tick(double seconds) override;
protected:
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

    Directions Visibility(const Position& p) const;
    double DiffusionRate(const Position& p, const Direction& d) const;
    void ComputeFlow(double seconds);
    void Flow(double seconds);
    void Diffuse(double seconds);
    bool IsBoundary(const Position& p) const;
private:
    class Data
    {
    public:
        Data();
        Data(unsigned longitude, unsigned latitude, unsigned altitude);
        Data(const Data&) = default;
        bool IsInside(const Position& p) const;
        Position GetPosition(const Voxel* pVoxel) const;
        const Voxel& operator[](const Position& p) const;
        Voxel& operator[](const Position& p);
        unsigned Latitude() const;
        unsigned Longitude() const;
        unsigned Altitude() const;
     
        template<class T, class ItT>
        class typed_iterator
        {
        public:
            typed_iterator(typename ItT it, const Position& limit, const Position& position) :
                it(it),
                limit(limit),
                position(position)
            {
            }
            typed_iterator<T, ItT>& operator++()
            {
                ++it;
                if (++position.z == limit.z)
                {
                    position.z = 0;
                    if (++position.y == limit.y)
                    {
                        position.y = 0;
                        if (++position.x == limit.x)
                        {
                            position.x = 0;
                        }
                    }
                }
                return *this;
            }
            //iterator operator++(int);
            bool operator==(const typed_iterator<T, ItT>& other) const
            {
                return it == other.it;
            }

            bool operator!=(const typed_iterator<T, ItT>& other) const { return !(*this == other); }
            
            template<typename PairType>
            struct VoxelPair
            {
                const Position& position;
                PairType& voxel;
                PairType* operator->() const { return &voxel;  }
            };
            VoxelPair<T> operator*()
            {
                return VoxelPair<T> { position,*it };
            }
            
            // iterator traits
            using difference_type = long;
            using value_type = VoxelPair<T>;
            using pointer = VoxelPair<T>*;
            using reference = VoxelPair<T>&;
            using iterator_category = std::forward_iterator_tag;
            
            ItT it;
            const Position limit;
            Position position;
        };
        using iterator = typed_iterator<Voxel, std::vector<Voxel>::iterator>;
        using const_iterator = typed_iterator<const Voxel, std::vector<Voxel>::const_iterator>;
        
        iterator begin() { return iterator(voxels.begin(), Position(longitude, latitude, altitude), Position(0, 0, 0)); }
        iterator end() { return iterator(voxels.end(), Position(longitude, latitude, altitude), Position(longitude, latitude, altitude)); }
        const_iterator begin() const{ return const_iterator(voxels.cbegin(), Position(longitude, latitude, altitude), Position(0, 0, 0)); }
        const_iterator end() const { return const_iterator(voxels.cend(), Position(longitude, latitude, altitude), Position(longitude, latitude, altitude)); }
    protected:
        Position Stride() const;
        unsigned Index(const Position& p) const;
    private:
        std::vector<Voxel> voxels;
        unsigned longitude, latitude, altitude;
    };
    Data voxels;
    double planetRadius;
    double gravity;
};

constexpr double PascalPerAtmosphere = 101325.0;

std::wistream& operator>>(std::wistream& s, VoxelMap& map);

}

