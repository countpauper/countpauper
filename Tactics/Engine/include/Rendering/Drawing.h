#pragma once
#include "Geometry/Coordinate.h"

namespace Engine
{
    Vector TransformBillboard(const Vector& v, const Matrix& m);
    void glVertex(const Vector& v);
    void glBillBoard(double width = 1.0, double height = 1.0);
    void glDrawArrow(const Vector& v, double size = 0.3, double angle = 0.5);
}
