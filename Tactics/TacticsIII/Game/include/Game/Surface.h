#pragma once 

#include "Game/Obstacle.h"
#include "Game/Statted.h"
#include "Game/Counted.h"
#include "Game/Equipped.h"
#include "Game/Conditions.h"

namespace Game 
{

class Material;

class Surface : 
    public Obstacle
{
public:
    Surface(const Material& material, float size=1.0f);

    // Scenery override
    void Render() const override;
    std::pair<double, std::uint32_t> Intersection(const Engine::Line& line) const override;

    // Actor Interface
    void Move(const class World& world, Position destination) override;
    Position GetPosition() const override;
    Size GetSize() const override;

    const Engine::Object& GetAppearance() const override;
    Statted& GetStats() override;
    const Statted& GetStats() const override;
    Counted& GetCounts() override;
    const Counted& GetCounts() const override;
    const class Equipped& GetEquipment() const override;
    class Equipped& GetEquipment() override;
    class Conditions& GetConditions() override;
    const class Conditions& GetConditions() const override;
private:
    const Material& material;
    float size;
    static NoStats stats;
    static NoCounts counts;
    static NoConditions conditions;
    static NoEquipment equipment;

};

}