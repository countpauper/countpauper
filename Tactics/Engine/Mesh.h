#pragma once

#include "Vector.h"
#include "Coordinate.h"
#include "Color.h"
#include <vector>
#include <set>

namespace Engine
{
struct Matrix;
struct AABB;

class Mesh
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

    void Render() const;
    AABB GetBoundingBox() const;
    Mesh& operator*=(const Matrix& transformation);

    std::vector<Vertex> vertices; 
    std::vector<Triangle> triangles;

    std::vector<uint32_t> names;    // same size as triangles. not together due to glDrawElements
    void SetName(uint32_t name);
    void SetColor(RGBA color);

    Mesh& operator+=(const Mesh& addition);

    void Degenerate();
    void Generate() const;
private:
    void GenerateVertexBuffer() const;
    void GenerateIndexBuffer() const;

    mutable uint32_t vertexBuffer, opaqueTriangleBuffer, translucentTriangleBuffer;
    mutable uint32_t opaqueTrianglesBuffered, translucentTrianglesBuffered;

    uint32_t* buffer = &vertexBuffer;
    const unsigned int buffers = unsigned int(1+(&translucentTriangleBuffer - &vertexBuffer));

    void RenderSelection() const;
    void RenderOpaque() const;
    void RenderTranslucent() const;
};

class Box: public Mesh
{
public:
    Box();
    explicit Box(const double size) :
        Box(Vector(size, size, size))
    {
    }
    explicit Box(const Vector& size);
};

}