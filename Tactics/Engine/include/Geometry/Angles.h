#pragma once
#include "Geometry/Vector.h"

namespace Engine
{
    constexpr double PI = 3.14159265358979323846;

    double Rad2Deg(double rad);
    double Deg2Rad(double rad);
    double ShortestTurn(double from, double to);

    std::pair<double, double> FaceYawPitch(Vector direction, Vector yawAxis=Vector(0,0,1));
}
