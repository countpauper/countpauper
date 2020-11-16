#pragma once
#include <string>
#include "IEnvironment.h"
// impl
#include "Box.h"

namespace Game
{

class StaticEnvironment : public IEnvironment
{
public:
    StaticEnvironment(const Size& size);

    void Fill(const Engine::IVolume& v, const Material& m) override;
    void ApplyForce(const Engine::IVolume& c, const Engine::Vector& v) override;
    void ApplyForce(const Engine::Coordinate& c, double force) override;
    void Heat(const Engine::Coordinate& c, double energy) override;

    void ConnectChange(ChangeSignal::slot_type slot) override;
    double Density(const Engine::IVolume& c) const override;
    double Temperature(const Engine::IVolume& c) const override;
    Engine::Vector Force(const Engine::IVolume& c) const override;
    const Material* GetMaterial(const Engine::Coordinate& c) const override;
private:
    struct Voxel
    {
        const Material* material;
        double temperature;
        double density;
    };
    class iterator
    {
    public:
        // iterate over all data
        explicit iterator(Voxel& v);
        iterator& operator++();
        //iterator operator++(int);
        bool operator==(const iterator& other) const;
        bool operator!=(const iterator& other) const;
        using value_type = Voxel;
        value_type& operator*() const;

        // iterator traits
        using difference_type = int;
        using pointer = const value_type*;
        using reference = const value_type&;
        using iterator_category = std::forward_iterator_tag;

        StaticEnvironment::Voxel* p;
    };
    iterator begin();
    iterator end();

    Voxel& operator[](const Position& pos);
    const Voxel& operator[](const Position& pos) const;
    Box GetBoundingBox() const;
    bool IsInside(const Position& pos) const;
    Size m_size;
    std::unique_ptr<Voxel[]> m_voxels;
};
}
#pragma once
