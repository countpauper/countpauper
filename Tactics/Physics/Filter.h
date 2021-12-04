#pragma once
#include <functional>

namespace Engine
{
    struct Coordinate;
}
namespace Physics
{
struct Material;

using Filter = const std::function<bool(const Engine::Coordinate& c, const Material* m, double temperature, double density)>&;
static Filter fillAll = [](const Engine::Coordinate&, const Material*, double, double) { return true; };

}
