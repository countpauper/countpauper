#pragma once

#include "Vector.h"
#include "Coordinate.h"
#include "Color.h"
#include "Volume.h"
#include "IRendition.h"
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
        RGBA color;
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
    mutable size_t opaqueTrianglesBuffered, translucentTrianglesBuffered;

    void RenderSelection() const;
    void RenderOpaque() const;
    void RenderTranslucent() const;
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