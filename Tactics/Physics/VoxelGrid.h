#pragma once
#include <string>
#include <array>
#include <map>
#include <sstream>
#include "DiscreteGrid.h"
#include "Physics/BoxIterator.h"

namespace Physics
{
    template<class _Voxel>
    class VoxelGrid : public DiscreteGrid
    {
    public:
        // size in meter, grid in meter/voxel
        VoxelGrid(const Engine::Vector& extent, const Grid& grid = Grid()) :
            DiscreteGrid(extent, grid)
        {
            data = std::vector<_Voxel>(size.Volume());
        }

        _Voxel& operator[](const Position& p)
        {
            return data.at(Index(p));
        }

        const _Voxel& operator[](const Position& p) const
        {
            return  data.at(Index(p));
        }

        size_t Fill(const Engine::IVolume& v, Filter filter, const Material& m, double temperature, std::optional<double> density = std::optional<double>())
        {
            size_t filled = 0;
            Box bounds = grid(v.GetBoundingBox()) & Bounds();

            if (!density.has_value())
                density = m.normalDensity;

            double normalDensity = m.normalDensity; //  Density(PascalPerAtmosphere, temperature);
            double fraction = *density / normalDensity;
            for (iterator it(*this, bounds); it != it.end(); ++it)
            {
                // TODO: for water it could be filled with amounts based on vertical levels for even smoother water surface
                auto center = grid.Center(it.position);
                auto& voxel = (*it).second;
                if ((v.Contains(center)) && (filter(center, voxel.GetMaterial(), voxel.Temperature(), voxel.Density())))
                {
                    voxel = PackedVoxel(m, temperature, fraction);
                    filled++;
                }
            }
            return filled;
        }

        void Constrain(const Engine::IVolume& v, const Material& m, double temperature, Function density)
        {
            constraints.emplace_back(Constraint(v, m, temperature, density));
        }

        void ApplyForce(const Engine::IVolume& c, const Engine::Vector& v) {}
        void Heat(const Engine::Coordinate& c, double energy) {}

        Box Heat(double seconds, const Position &position, _Voxel& current)
        {
            Directions halfDirs = Direction::up | Direction::north | Direction::west;
            Box invalid;
            Box bounds = Bounds();
            double t = current.Temperature();
            const auto& material = current.GetMaterial();
            double mass = current.Mass(grid.Volume());
            double energy = mass * t * material->heatCapacity;

            for (auto dir : halfDirs)
            {
                auto dirPosition = position + dir.Vector();
                if (!bounds.Contains(dirPosition))
                    continue;
                auto& neighbour = (*this)[dirPosition];
                double nt = neighbour.Temperature();
                if (t == nt)
                    continue;

                const auto& neighbour_mat = neighbour.GetMaterial();
                double neighbour_mass = neighbour.Mass(grid.Volume());
                double neighbour_energy = neighbour_mass * nt * neighbour_mat->heatCapacity;

                double surface = dir.Surface(grid);
                Engine::Vector v(t, nt, 0);
                auto rateFunction = [&](double, const Engine::Vector& t)
                {
                    // use conducivity of coldest, based on nothing except a hunch that it's dominant to spread the energy from the interface
                    auto conductivity = (t.x < t.y) ? material->thermalConductivity : neighbour_mat->thermalConductivity;
                    double exchangeRate = surface * conductivity * (t.y - t.x);
                    double dx = exchangeRate / (mass*material->heatCapacity);
                    double dy = -exchangeRate / (neighbour_mass * neighbour_mat->heatCapacity);
                    return Engine::Vector(dx, dy, 0);
                };
                Engine::Vector newState = Engine::RungeKutta<Engine::Vector>(0, v, seconds, rateFunction);
                current.SetTemperature(newState.x);
                neighbour.SetTemperature(newState.y);
                invalid |= position;
                invalid |= dirPosition;
            }

            return invalid;
        }

        double Density(const Engine::IVolume& c) const
        {
            return 0.0;
        }

        double Temperature(const Engine::IVolume& v) const
        {
            Box bounds = grid(v.GetBoundingBox()) & Bounds();
            double count = 0.0;
            double total = 0.0;
            for (const_iterator it(*this, bounds); it != it.end(); ++it)
            {
                if (v.Contains(grid.Center(it.position)))
                {
                    count++;
                    total += (*it).second.Temperature();
                }
            }
            if (count)
                return total / count;
            else
                return 0;
        }

        Engine::Vector Force(const Engine::Coordinate& c) const
        {
            return Engine::Vector::zero;
        }

        const Material* GetMaterial(const Engine::Coordinate& c) const
        {
            auto p = grid(c);
            if (Bounds().Contains(p))
            {
                return (*this)[p].GetMaterial();
            }
            else
            {
                return nullptr;
            }
        }

        Engine::RGBA Color(const Engine::Line& l) const
        {
            Position p = grid(l.b);
            if (Bounds().Contains(p))
            {
                return (*this)[p].Color();
            }
            else
            {
                return Engine::RGBA();
            }
        }

        double Measure(const Material* material, const Engine::IVolume& in = Engine::AABB::infinity) const
        {
            double volume = 0.0;
            auto bounds = grid(in.GetBoundingBox()) & Bounds();

            if (material == &Material::vacuum)
            {
                for (const_iterator it(*this, bounds); it != it.end(); ++it)
                {
                    if (((*it).second.GetMaterial() == material) &&
                        (in.Contains(grid.Center(it.position))))
                        volume += grid.Volume();
                }
            }
            else
            {
                for (const_iterator it(*this, bounds); it != it.end(); ++it)
                {
                    if (((*it).second.GetMaterial() == material) &&
                        (in.Contains(grid.Center(it.position))))
                        volume += grid.Volume() * (*it).second.Amount() / PackedVoxel::normalAmount;
                }
            }
            return volume;
        }

        std::wstring Statistics() const
        {
            std::wstringstream str;
            str << size.Volume() * sizeof(data[0]) / 1024 << "kB";
            return str.str();
        }

        using iterator = BoxDataIterator<VoxelGrid<_Voxel>, _Voxel>;
        using const_iterator = BoxDataIterator<const VoxelGrid<_Voxel>, const _Voxel>;
    private:

        std::vector<_Voxel> data;
    };
}
