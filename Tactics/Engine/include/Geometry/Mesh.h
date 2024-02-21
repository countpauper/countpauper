#pragma once

#include "Geometry/Vector.h"
#include "Geometry/Coordinate.h"
#include "Rendering/Color.h"
#include "Geometry/Volume.h"
#include "Rendering/IRendition.h"
#include <vector>
#include <set>

namespace Engine
{
struct Matrix;
struct AABB;

class Mesh : public IVolume, public IRendition, public Clone<Mesh>
{
public:
    Mesh();
    virtual ~Mesh();

    struct TextureCoordinate
    {
        double x, y, z;
    };
    struct Vertex
    {
        Coordinate c;
        Vector n;
        TextureCoordinate t;
        RGBA color{0};
    };

    struct Triangle
    {
        uint32_t vertex[3];
    };

    void Render() const override;
    AABB GetBoundingBox() const override;
    double Volume() const override;

    Mesh& operator*=(const Matrix& transformation);

    std::vector<Vertex> vertices;
    std::vector<Triangle> triangles;

    std::vector<uint32_t> names;    // same size as triangles. not together due to glDrawElements
    void SetName(uint32_t name);
    void SetColor(RGBA color);
    std::pair<double, uint32_t> NamedIntersection(const Line& line) const;
    double Intersection(const Line& line) const;

    Mesh& operator+=(const Mesh& addition);
    double Distance(const Coordinate& p) const override;

    void Invalidate();
    void InvalidateVertices();
    void InvalidateTriangles();
private:
    void Validate() const;
    void GenerateVertexBuffer() const;
    void GenerateIndexBuffer() const;

    mutable uint32_t vertexBuffer, opaqueTriangleBuffer, translucentTriangleBuffer;
    mutable uint32_t opaqueTrianglesBuffered, translucentTrianglesBuffered;

    static void SetupVertexPointers();
    void RenderSelection() const;
    void RenderOpaque() const;
    void RenderTranslucent() const;
};


class Quad : public Mesh
{
public:
    Quad(Coordinate a, Coordinate b, Coordinate c, Coordinate d);
};

class Box: public Mesh
{
public:
    Box();
    Box(const AABB& bounds);
    explicit Box(const double size) :
        Box(Vector(size, size, size))
    {
    }
    explicit Box(const Vector& size);
};

}
