#pragma once
#include <string>
#include <array>
#include <map>
#include <sstream>
#include "DiscreteGrid.h"
#include "Physics/BoxIterator.h"
#include "Engine/Utils.h"

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

        size_t Fill(const Engine::IVolume& v, Filter filter, const Material& m, double temperature, std::optional<double> density = std::optional<double>()) override
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
                    voxel = _Voxel(m, temperature, fraction);
                    filled++;
                }
            }
            return filled;
        }

        void ApplyForce(const Engine::IVolume& c, const Engine::Vector& v) override {}
        void Heat(const Engine::IVolume& c, double energy) override {}

        Box Heat(double seconds, const Position &position, _Voxel& current)
        {
            Directions halfDirs = Direction::up | Direction::north | Direction::west;
            Box invalid;
            Box bounds = Bounds();
            double t = current.Temperature();
            const auto& material = current.GetMaterial();
            double mass = current.Density() * grid.Volume();
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
                double neighbour_mass = neighbour.Density() * grid.Volume();
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

        double Mass(const Engine::IVolume& in) const override
        {
            double volume = 0.0;
            auto bounds = grid(in.GetBoundingBox()) & Bounds();

            for (const_iterator it(*this, bounds); it != it.end(); ++it)
            {
                if (in.Contains(grid.Center(it.position)))  
                    volume += (*it).second.Density() * grid.Volume(); // TODO: not entirely accurate, should measure overlap % with grid volume
            }
            return volume;
        }

        double Temperature(const Engine::IVolume& v) const override
        {
            Box bounds = grid(v.GetBoundingBox()) & Bounds();
            double count = 0.0;
            double total = 0.0;
            for (const_iterator it(*this, bounds); it != it.end(); ++it)
            {
                if (v.Contains(grid.Center(it.position)))   // TODO: not entirely accurate, should measure overlap % and energy and don't divide by count but volume
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

        Engine::Vector Force(const Engine::Coordinate& c) const override
        {
            return Engine::Vector::zero;
        }

        const Material* GetMaterial(const Engine::Coordinate& c) const override
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

        Engine::RGBA Color(const Engine::Line& l) const override
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

        double Measure(const Material* material, const Engine::IVolume& in = Engine::AABB::infinity) const override
        {
            double volume = 0.0;
            auto bounds = grid(in.GetBoundingBox()) & Bounds();

            for (const_iterator it(*this, bounds); it != it.end(); ++it)
            {
                if (in.Contains(grid.Center(it.position)))
                    volume += (*it).second.Measure(material);
            }
            return volume;
        }

        std::string Statistics() const override
        {
            std::stringstream str;
            // TODO: chrono duration and format as string?
            str << "Time: " << Engine::FormatDuration(time).c_str();
            str << " Mass: " << Mass(Engine::AABB::infinity)/1000.0 << "kg";
            str << " Temperature: " << Temperature(Engine::AABB::infinity) << "K";
            str << " Memory: " << size.Volume() * sizeof(data[0]) / 1024 << "kB";
            return str.str();
        }

        using iterator = BoxDataIterator<VoxelGrid<_Voxel>, _Voxel>;
        using const_iterator = BoxDataIterator<const VoxelGrid<_Voxel>, const _Voxel>;
    private:

        std::vector<_Voxel> data;
    };
}
