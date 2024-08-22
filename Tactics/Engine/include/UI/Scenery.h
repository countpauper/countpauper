#pragma once
#include <cstdint>
#include "UI/Object.h"
#include "UI/Camera.h"
#include "Utility/Range.h"

namespace Engine
{
class Mesh;

class Scenery : public Object
{
public:
    explicit Scenery(Mesh& mesh);
    void Render() const override;
    Coordinate GetCoordinate() const override;
    Quaternion GetOrientation() const override;

    std::pair<double, std::uint32_t> Intersection(const Line& line) const;
private:
    Mesh& mesh;
};


}
