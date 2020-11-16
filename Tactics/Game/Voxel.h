#pragma once
#include <string>
#include "Position.h"
#include "Material.h"
#include "Direction.h"
#include "Map.h"    // only for square, TODO split

namespace Game
{

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
private:
    void RenderFaces(const Position& p, const Directions& visibility, unsigned mode) const;
    void RenderFace(const Position& p, Direction direction, unsigned mode) const;
};
}
#pragma once
