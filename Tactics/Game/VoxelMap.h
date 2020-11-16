#pragma once
#include "Map.h"
#include "VoxelData.h"
#include "Material.h"
#include "Engine/Vector.h"
#include "Engine/Line.h"
#include "Plane.h"
#include "Box.h"
#include <string>

namespace Engine { class Mesh;  struct AABB; }
namespace Game
{

class VoxelMap : public Map
{
public:
    VoxelMap();
    VoxelMap(unsigned longitude, unsigned latitude, unsigned altitude);
    ~VoxelMap();

    class Data : public VoxelData
    {
    public:
        Data();
        Data(unsigned longitude, unsigned latitude, unsigned altitude);
        Data(const Data&) = default;
        Section In(const Engine::AABB& meters) const;

        Engine::Vector FluxGradient(const Position& position) const;   // in g/m2
        Engine::Vector DensityGradient(const Position& position) const;   // in g/m2
        Engine::Vector Flow(const Position& position) const;  // in meters/second
        double Density(const Engine::Coordinate& c) const;
        double Temperature(const Engine::Coordinate& c) const;

        class Flux
        {
        public:
            Flux(const Direction axis, unsigned longitude, unsigned latitude, unsigned altitude);
            float operator[](const Position& p) const;
            float operator()(int x, int y, int z) const { return (*this)[Position(x, y, z)]; }
            float& operator[](const Position& p);
            double Gradient(const Position& p) const;
            bool IsOffset(const Direction& d) const;
            void SetBoundary(const Position& p, const Directions& boundary, double boundaryFlux);
            float Extrapolate(const Position& outsidePosition, const Directions& boundary, double boundaryFlux) const;
            Box Bounds() const;
            Box Edge(const Direction& dir) const;
            Box ExtendedEdge(const Direction& dir) const;

            friend class iterator;
            class iterator
            {
            public:
                iterator(const Flux& data, const Position& start);
                iterator(const Flux& data, const Box& box);

                iterator& operator++();
                bool operator==(const iterator& other) const;
                bool operator!=(const iterator& other) const { return !((*this) == other); }
                using value_type = std::pair<Position, float>;
                value_type operator*() const;
                Directions IsBoundary() const;

                // iterator traits
                using difference_type = Position;
                using pointer = const float*;
                using reference = const float&;
                using iterator_category = std::forward_iterator_tag;

                const Flux& data;
                const Box box;
                Position position;
            };
            iterator begin() const;
            iterator end() const;

            class Section
            {
            public:
                Section(const Flux& data, const Box& box);
                iterator begin() const;
                iterator end() const;
                using value_type = iterator::value_type;
                const Flux& data;
                const Box box;
            };
            class Boundary : public Section
            {
            public:
                Boundary(const Flux& data, const Direction& direction);
            };
            class Corner : public Section
            {
            public:
                Corner(const Flux& data, const Directions& directions);
            };

            Section BoundaryCondition(const Direction& dir) const;
            Section BoundaryCondition(const Directions& boundary) const;
        private:
            unsigned Index(const Position& p) const;
            Directions IsBoundary(const Position& p) const;
            void InvalidateCorners();

            Direction axis;
            Position offset;
            Size size;
            std::vector<float> flux;
        };
        const Flux& U() const { return u; }
        Flux& U() { return u; }
        const Flux& V() const { return v; }
        Flux& V() { return v; }
        const Flux& W() const { return w; }
        Flux& W() { return w; }

    protected:
        void InvalidateCorners();
    private:
        Flux u, v, w;                       // flux (/s*m2)
    };
    // Generate
    void Space(unsigned x, unsigned y, unsigned z); // Day 0 there is nothing
    void World(double radius);
    void Air(double temperature, double meters);     // Day 1 the sky
    void Wind(const Engine::Vector& speed);     // direction in meter/second
    void Sea(int level, double temperature);      // Day 2 Separate the water from the sky 
    void Hill(const Engine::Line& ridgeLine, double stddev);
    void Wall(const Engine::Line& bottomLine, double height, double thickness);
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
    static Engine::Coordinate Center(const Position& grid);
protected:
    void GenerateMesh();
    void RenderPretty() const;
    void RenderAnalysis() const;
    void SetDensityToMeshColor();


    void Flow(double seconds);
    void FluxBoundary();
    void FluxBoundary(const Directions boundary);
    void Continuity(double seconds);
    void GridBoundary();

    Directions Visibility(const Position& p) const;
    double Elevation(int z) const;
    float AtmosphericTemperature(double elevation) const;
    float AtmosphericPressure(double elevation) const;
    double Density(const Engine::Coordinate& c) const;   // c in meters, density interpolated
    double Temperature(const Engine::Coordinate& c) const;   // c in meters, temperature interpolated
protected:
    static constexpr double dX = HorizontalEl * MeterPerEl;
    static constexpr double dY = HorizontalEl * MeterPerEl;
    static constexpr double dZ = VerticalEl * MeterPerEl;
    static Position Grid(const Engine::Coordinate& meters);
    Data voxels;
    std::unique_ptr<Engine::Mesh> mesh;
    double time;
    double planetRadius;    // m
    double atmosphereRadius; // m
    double gravity;         // m/s^2
    float atmosphericTemperature;   // K at 0
    Engine::Vector wind;
};

std::wistream& operator>>(std::wistream& s, VoxelMap& map);
VoxelMap::Data::Flux::iterator::difference_type operator-(const VoxelMap::Data::Flux::iterator& a, const VoxelMap::Data::Flux::iterator& b);

}

