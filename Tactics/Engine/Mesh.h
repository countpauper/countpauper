#pragma once

#include "Vector.h"
#include "Coordinate.h"
#include "Color.h"
#include <vector>

namespace Engine
{
struct Matrix;
struct AABB;

class Mesh
{
public:
    Mesh() = default;

    struct TextureCoordinate
    {
        double s, t;
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
        uint32_t name;
    };

    void Render() const;
    AABB GetBoundingBox() const;
    Mesh& operator*=(const Matrix& transformation);

    std::vector<Vertex> vertices; 
    std::vector<Triangle> triangles;
    std::vector<uint32_t> opaque;   // opaque triangle indices
    std::vector<uint32_t> translucent;  // translucent triangle indices
    void SetName(uint32_t name);
private:
    uint32_t vertex_buffer, triangle_buffer;
    void Invalidate();
    void Generate();

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