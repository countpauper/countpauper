#include <GL/glew.h>
#include <GL/gl.h>
#include "Geometry/Mesh.h"
#include "Geometry/Angles.h"
#include "Geometry/Matrix.h"
#include "Geometry/AxisAlignedBoundingBox.h"
#include "Geometry/Quaternion.h"
#include "Geometry/Triangle.h"
#include "Utility/GLUtil.h"
#include "Geometry/Plane.h"
#include "Geometry/Triangle.h"
#include "UI/Debug.h"
#include "Utility/Error.h"

#include <array>
#include <cassert>

namespace Engine
{

Mesh::Mesh() :
    vertexBuffer(0),
    opaqueTriangleBuffer(0),
    opaqueTrianglesBuffered(0),
    translucentTriangleBuffer(0),
    translucentTrianglesBuffered(0)
{
}

Mesh::~Mesh()
{
    Invalidate();
}

void Mesh::Render() const
{
    Validate();

    if (glGet<int>(GL_RENDER_MODE) == GL_SELECT)
    {
        RenderSelection();
    }
    else if (glIsEnabled(GL_BLEND))
    {
        RenderTranslucent();
    }
    else
    {
        RenderOpaque();
    }
}

template<typename T, typename U> constexpr size_t offsetOf(U T::*member)
{
    T* v;
    return (std::byte*)&(v->*member) - (std::byte*)v;
}

void Mesh::SetupVertexPointers()
{
    assert(3 * sizeof(GLdouble) == sizeof(Vertex::c));
    glVertexPointer(3, GL_DOUBLE, sizeof(Vertex), (void*)offsetOf(&Vertex::c));
    glTexCoordPointer(3, GL_DOUBLE, sizeof(Vertex), (void*)offsetOf(&Vertex::t));
    glNormalPointer(GL_DOUBLE, sizeof(Vertex), (void*)offsetOf(&Vertex::n));
    glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(Vertex), (void*)offsetOf(&Vertex::color));
}

void Mesh::RenderOpaque() const
{
    if (!opaqueTrianglesBuffered)
        return;

    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    //glVertexAttribPointer(0, 3, GL_DOUBLE, GL_FALSE, 0, 0);
    SetupVertexPointers();

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, opaqueTriangleBuffer);
    glDrawElements(GL_TRIANGLES, 3 * opaqueTrianglesBuffered, GL_UNSIGNED_INT, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    /* Last minute generate
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);

    assert(3 * sizeof(GLdouble) == sizeof(Vertex::c));
    glVertexPointer(3, GL_DOUBLE, sizeof(Vertex), &vertices.front().c);
    if (glIsEnabled(GL_TEXTURE_3D))
    {
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        glTexCoordPointer(3, GL_DOUBLE, sizeof(Vertex), &vertices.front().t);
    }
    else if (glIsEnabled(GL_TEXTURE_2D))
    {
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        glTexCoordPointer(2, GL_DOUBLE, sizeof(Vertex), &vertices.front().t);
    }
    else
    {
        glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    }

    if (glIsEnabled(GL_LIGHTING))
    {
        glEnableClientState(GL_NORMAL_ARRAY);
        glNormalPointer(GL_DOUBLE, sizeof(Vertex), &vertices.front().n);
    }
    else
    {
        glDisableClientState(GL_NORMAL_ARRAY);
    }

    glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(Vertex), &vertices.front().color);
    glDrawElements(GL_TRIANGLES, 3*opaqueTriangles.size(), GL_UNSIGNED_INT, opaqueTriangles.data());
*/

/*
glBegin(GL_TRIANGLES);
    for (auto t : triangles)
    {
        for (int i = 0; i < 3; ++i)
        {
            const auto& v = vertices.at(t.vertex[i]);
            glNormal3d(v.n.x, v.n.y, v.n.z);
            glTexCoord3d(v.t.x, v.t.y, v.t.z);
            glColor4ub(v.color.r, v.color.g, v.color.b, v.color.a);
            glVertex3d(v.c.x, v.c.y, v.c.z);
        }
    }
    glEnd();
*/
    CheckGLError();
}

void Mesh::RenderTranslucent() const
{
    if (!translucentTrianglesBuffered)
        return;

    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    SetupVertexPointers();
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, translucentTriangleBuffer);
    glDrawElements(GL_TRIANGLES, 3 * translucentTrianglesBuffered, GL_UNSIGNED_INT, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

   /*
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);

    assert(3 * sizeof(GLdouble) == sizeof(Vertex::c));
    glVertexPointer(3, GL_DOUBLE, sizeof(Vertex), &vertices.front().c);
    glTexCoordPointer(2, GL_DOUBLE, sizeof(Vertex), &vertices.front().t);
    glNormalPointer(GL_DOUBLE, sizeof(Vertex), &vertices.front().n);
    glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(Vertex), &vertices.front().color);
    glDrawElements(GL_TRIANGLES, 3 * translucentTriangles.size(), GL_UNSIGNED_INT, translucentTriangles.data());
*/
}

void Mesh::RenderSelection() const
{
    glPushName(0);
    for (auto t : triangles)
    {
        assert(names.empty());  // unused
        glBegin(GL_TRIANGLES);
        for (int i = 0; i < 3; ++i)
        {
            const auto& v = vertices.at(t.vertex[i]);
            glVertex3d(v.c.x, v.c.y, v.c.z);
        }
        glEnd();
    }
    glPopName();
}

AABB Mesh::GetBoundingBox() const
{
    double inf = std::numeric_limits<double>::infinity();
    AABB box(Coordinate(inf, inf, inf), Coordinate(-inf, -inf, -inf));
    for (auto& vertex : vertices)
    {
        box |= vertex.c;
    }
    return box;
}


bool Mesh::IsNamed() const
{
    return triangles.empty() || !names.empty();
}

void Mesh::SetName(uint32_t name)
{
    names.resize(triangles.size(), name);
}

void Mesh::SetColor(RGBA color)
{
    for (auto& v : vertices)
    {
        v.color = color;
    }
    Invalidate();   // also triangles due to opaque/translucent separation
}

void Mesh::SetColor(uint32_t name, RGBA color)
{
    if (!IsNamed())
    {
        throw std::runtime_error("Color selection set to a mesh without names");
    }
    unsigned tri = 0;
    for(auto n: names)
    {
        if (n == name)
        {
            const Triangle& triangle = triangles[tri];
            for(auto v : triangle.vertex)
                vertices[v].color = color;
        }
        ++tri;
    }
    Invalidate();   // also triangles due to opaque/translucent separation
}

std::pair<double, uint32_t> Mesh::NamedIntersection(const Line& line) const
{
    // TODO: optimize by checking the intersection of the line with the axis aligned bounding box
    // but optimize the AABB by precomputing in Validate() and reset in Invalidate()
    // and make vertices private, provide operator[] etc
    unsigned idx = 0;
    double nearest = std::numeric_limits<double>::max();
    unsigned best = names.size();
    for(auto triangle : triangles)
    {
        Engine::Triangle t(vertices[triangle.vertex[0]].c,
            vertices[triangle.vertex[1]].c,
            vertices[triangle.vertex[2]].c);
        double distance = t.Intersection(line);
        if (distance>=0.0 && distance < nearest) // nb could be nan
        {
            nearest = distance;
            best = idx;
        }
        ++idx;
    }
    uint32_t name = (best < names.size()) ? names[best] : 0;
    return std::make_pair(nearest, name);
}

double Mesh::Intersection(const Line& line) const
{
    return NamedIntersection(line).first;
}

double Mesh::Distance(const Coordinate& p) const
{
    double minDistance = std::numeric_limits<double>::infinity();
    for (auto& t : triangles)
    {   // pick the nearest point on the nearest triangle that the point is in front (outside) of
        //  if the point is behind all faces then take the nearest point from the surface
        auto distance = Engine::Triangle(vertices[t.vertex[0]].c, vertices[t.vertex[1]].c, vertices[t.vertex[2]].c).Distance(p);
        if (std::abs(distance) < std::abs(minDistance))
        {
            minDistance = distance;
        }
    }
    return minDistance;
}

double Mesh::Volume() const
{
    // http://chenlab.ece.cornell.edu/Publication/Cha/icip01_Cha.pdf
    throw std::runtime_error("Volume of arbitrary mesh is unimplemented");
}

Mesh& Mesh::operator+=(const Mesh& addition)
{
    Invalidate();
    assert(vertices.size() < std::numeric_limits<uint32_t>::max());
    uint32_t vertexOffset = uint32_t(vertices.size());
    vertices.insert(vertices.end(), addition.vertices.begin(), addition.vertices.end());
    assert(triangles.empty() || IsNamed() == addition.IsNamed()); // no functionality to add unnamed to named
    names.insert(names.end(), addition.names.begin(), addition.names.end());
    for (const auto& t : addition.triangles)
    {
        triangles.push_back(Triangle{ t.vertex[0] + vertexOffset, t.vertex[1] + vertexOffset, t.vertex[2] + vertexOffset });
    }
    return *this;
}

Mesh& Mesh::operator*=(const Matrix& transformation)
{
    InvalidateVertices();
    for (auto& vertex : vertices)
    {
        vertex.c *= transformation;
        vertex.n *= transformation.Rotation();
    }
    return *this;
}


void Mesh::Invalidate()
{
    InvalidateVertices();
    InvalidateTriangles();
}

void Mesh::InvalidateVertices()
{
    if (!vertexBuffer)
        return;
    glDeleteBuffers(1, &vertexBuffer);
    vertexBuffer = 0;
}
void Mesh::InvalidateTriangles()
{
    if (opaqueTriangleBuffer)
    {
        glDeleteBuffers(1, &opaqueTriangleBuffer);
        opaqueTriangleBuffer = 0;
        opaqueTrianglesBuffered = 0;
    }
    if (translucentTriangleBuffer)
    {
        glDeleteBuffers(1, &translucentTriangleBuffer);
        translucentTriangleBuffer = 0;
        translucentTrianglesBuffered = 0;
    }
}


void Mesh::Validate() const
{
    GenerateVertexBuffer();
    GenerateIndexBuffer();
}

void Mesh::GenerateVertexBuffer() const
{
    if (vertexBuffer)
        return;
    glGenBuffers(1, &vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, vertices.size()*sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

    glEnableClientState(GL_VERTEX_ARRAY);
    SetupVertexPointers();
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Mesh::GenerateIndexBuffer() const
{
    if (opaqueTriangleBuffer)
    {
        assert(translucentTriangleBuffer != 0);
        return;
    }

    assert(translucentTriangleBuffer == 0);

    std::vector<Triangle> opaqueTriangles;
    std::vector<Triangle> translucentTriangles;

    for (const auto& t : triangles)
    {
        RGBA c0 = vertices.at(t.vertex[0]).color;
        RGBA c1 = vertices.at(t.vertex[1]).color;
        RGBA c2 = vertices.at(t.vertex[2]).color;

        if (!c0.IsVisible() && !c1.IsVisible() && !c2.IsVisible())
            continue;   // transparent
        if (c0.IsOpaque() && c1.IsOpaque() && c2.IsOpaque())
        {
            opaqueTriangles.push_back(t);
        }
        else
        {
            translucentTriangles.push_back(t);
        }
    }
    opaqueTrianglesBuffered = opaqueTriangles.size();
    translucentTrianglesBuffered = translucentTriangles.size();

    glGenBuffers(1, &opaqueTriangleBuffer);
    glGenBuffers(1, &translucentTriangleBuffer);

    if (!opaqueTriangles.empty())
    {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, opaqueTriangleBuffer);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, opaqueTriangles.size() * sizeof(Triangle), opaqueTriangles.data(), GL_STATIC_DRAW);
    }

    if (!translucentTriangles.empty())
    {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, translucentTriangleBuffer);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, translucentTriangles.size() * sizeof(Triangle), translucentTriangles.data(), GL_STATIC_DRAW);
    }
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}


Quad::Quad(Coordinate a, Coordinate b, Coordinate c, Coordinate d)
{
    Vector normal = Plane(a, b, c).Normalized().normal;
    vertices.emplace_back(Vertex{ a, normal, TextureCoordinate{0,0,0}, RGBA::white});
    vertices.emplace_back(Vertex{ b, normal, TextureCoordinate{0,0,0}, RGBA::white});
    vertices.emplace_back(Vertex{ c, normal, TextureCoordinate{0,0,0}, RGBA::white});
    vertices.emplace_back(Vertex{ d, normal, TextureCoordinate{0,0,0}, RGBA::white});

    triangles.emplace_back(Triangle{0, 1, 2});
    triangles.emplace_back(Triangle{0, 2, 3});
}

Box::Box(const Vector& size)
    : Box()
{
    (*this) *= Matrix::Scale(size);
}

Box::Box(const AABB& bounds) :
    Box()
{
    (*this) *= Matrix::Scale(bounds.Extent());
    (*this) *= Matrix::Translation(Engine::Vector(bounds.Center()));
}


// TODO: this could also just be a static const
Box::Box()
{
    Coordinate a(0,0,0), b(0,0,1), c(0,1,1), d(0,1,0),
        e(1,0,0), f(1,0,1), g(1,1,1), h(1,1,0);
    (*this) += Quad(a, b, c, d); // x = 0
    (*this) += Quad(h, g, f, e); // x = 1
    (*this) += Quad(a, e, f, b); // y = 0
    (*this) += Quad(c, g, h, d); // y = 1
    (*this) += Quad(a, d, h, e);    // z = 0
    (*this) += Quad(b, f, g, c);    // z = 1
}

}

