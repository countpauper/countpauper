#pragma once
#include "Map.h"
#include "Material.h"
#include "Engine/Vector.h"
#include "Engine/Line.h"
#include "Plane.h"
#include "Box.h"
#include <string>

namespace Engine { class Mesh;  struct AABB; }
namespace Game
{

class FluidDynamics : public Map
{
public:
    FluidDynamics();
    FluidDynamics(unsigned longitude, unsigned latitude, unsigned altitude);
    ~FluidDynamics();

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
        void Render(const Position& p, const Directions& visibility) const;
        void RenderAnalysis(const Position& p, const Directions& visibility, const Engine::Vector& densityGradient) const;

        const Material* material;
        float temperature;      // Kelvin
        float density;          // gram/Liters
                                //const Engine::Vector flow;    // meter/second
    private:
        void RenderFaces(const Position& p, const Directions& visibility, unsigned mode) const;
        void RenderFace(const Position& p, Direction direction, unsigned mode) const;
    };

    class Data
    {
    public:
        Data();
        Data(unsigned longitude, unsigned latitude, unsigned altitude);
        Data(const Data&) = default;
        bool IsInside(const Position& p) const;
        const FluidDynamics::Voxel& operator[](const Position& p) const;
        FluidDynamics::Voxel& operator[](const Position& p);
        class iterator
        {
        public:
            // iterate over all data
            explicit iterator(const Data& data);
            iterator(const Data& data, const Box& box);
            iterator(const Data& data, const Position& position, const Box& box);
            iterator& operator++();
            //iterator operator++(int);
            bool operator==(const iterator& other) const;
            bool operator!=(const iterator& other) const;
            using value_type = std::pair<Position, Voxel>;
            value_type operator*() const;

            // iterator traits
            using difference_type = Position;
            using pointer = const value_type*;
            using reference = const value_type&;
            using iterator_category = std::forward_iterator_tag;

            const Data& data;
            Position position;
            const Box box;
        };

        using value_type = iterator::value_type;

        iterator begin() const { return iterator(*this); }
        iterator end() const { return iterator(*this, Insides().End(), Insides()); }

        class Section
        {
        public:
            Section(const Data& data, const Box& box);
            Section(const Data& data, const Engine::AABB& meters);

            iterator begin() const;
            iterator end() const;
            const Data& data;
            const Box box;
        };
        class Boundary : public Section
        {
        public:
            Boundary(const Data& data, const Direction& direction);
            Direction direction;
        };
        class Corner : public Section
        {
        public:
            Corner(const Data& data, const Directions& directions);
        };
        Boundary BoundaryCondition(const Direction& dir) const { return Boundary(*this, dir); }


        Section In(const Engine::AABB& meters) const;
        Section All() const;
        Box Insides() const;
        Box Bounds() const;
        Box Edge(const Direction& direction) const;
        Box ExtendedEdge(const Direction& direction) const;

        Position Clip(const Position& p) const;

        void SetPressure(const Position& location, const Material& material, double temperature, double pressure);
        void AdjustGrid(const Position& location, double temperature, double density);

        unsigned Latitude() const;
        unsigned Longitude() const;
        unsigned Altitude() const;
        Directions IsBoundary(const Position& position) const;
        Engine::Vector FluxGradient(const Position& position) const;   // in g/m2
        Engine::Vector DensityGradient(const Position& position) const;   // in g/m2
        Engine::Vector Flow(const Position& position) const;  // in meters/second
        float Density(const Position& position) const;
        float Temperature(const Position& position) const;
        const Material& MaterialAt(const Position& position) const;
        void SetDensity(const Position& p, float density);
        double Density(const Engine::Coordinate& c) const;   // c in meters, density interpolated
        double Temperature(const Engine::Coordinate& c) const;   // c in meters, temperature interpolated

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
        Position Stride() const;
        unsigned GridIndex(const Position& p) const;
        unsigned UncheckedGridIndex(const Position& p) const;
        void SetInvalid(const Position& position);
        void InvalidateCorners();
    private:
        std::vector<Voxel> voxels;  // longitude+2, latitude+2, altitude+2
        Flux u, v, w;                       // flux (/s*m2)
        Size size;
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

std::wistream& operator>>(std::wistream& s, FluidDynamics& map);
FluidDynamics::Data::iterator::difference_type operator-(const FluidDynamics::Data::iterator& a, const FluidDynamics::Data::iterator& b);
FluidDynamics::Data::Flux::iterator::difference_type operator-(const FluidDynamics::Data::Flux::iterator& a, const FluidDynamics::Data::Flux::iterator& b);

}
