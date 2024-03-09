#pragma once
#include <cstdint>
#include "UI/Object.h"
#include "Geometry/Coordinate.h"
#include "Geometry/Quaternion.h"
#include "UI/Camera.h"
#include "Utility/Range.h"

namespace Engine
{
class Mesh;

class Scenery : public Object
{
public:
    explicit Scenery(Mesh& mesh);
    virtual void Render() const;
    std::pair<double, std::uint32_t> Intersection(const Line& line) const;
    virtual Coordinate GetCoordinate() const;
    virtual Quaternion GetOrientation() const;
private:
    Mesh& mesh;
};


}
