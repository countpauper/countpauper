#pragma once
#include <string>
#include "Material.h"

namespace Physics
{

struct Voxel
{
    bool Solid() const;
    bool Gas() const;
//    Square Square(unsigned height) const;
    double Density() const;
    double Mass(double volume) const;   // volume: liter, mass: gram
    double Pressure() const;
 //   double Molecules(double volume) const;  // volume liters, amount: mole
    double Viscosity() const;
    double Hardness() const;
    double DiffusionCoefficient(const Voxel& to) const;

    double Translucency(double distance) const;    // I(x) = I0 * Translucency(), distance in meter
    bool Opaque(double distance) const; 
    bool Transparent(double distance) const;
    Engine::RGBA Color(double distance) const;
/*
    void Render(const Position& p, const Directions& visibility) const;
    void RenderAnalysis(const Position& p, const Directions& visibility, const Engine::Vector& densityGradient) const;
*/
    const Material* material;
    float temperature;      // Kelvin
    float density;          // gram/Liters
/*
private:
    void RenderFaces(const Position& p, const Directions& visibility, unsigned mode) const;
    void RenderFace(const Position& p, Direction direction, unsigned mode) const;
*/
};
}
#pragma once
