#include "stdafx.h"
#include "Mesh.h"
#include "Geometry.h"
#include "Matrix.h"
#include "AxisAlignedBoundingBox.h"
#include "Quaternion.h"
#include <GL/glew.h>
#include "GLutil.h"
#include <array>

namespace Engine
{

Mesh::Mesh() :
    opaqueTrianglesBuffered(0),
    translucentTrianglesBuffered(0)
{
    ZeroMemory((void*)buffer, buffers * sizeof(uint32_t));
}

Mesh::~Mesh()
{
    Degenerate();
}

void Mesh::Render() const
{
    Generate();

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

void Mesh::RenderOpaque() const
{
    if (!opaqueTrianglesBuffered)
        return;

    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
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
}

void Mesh::RenderTranslucent() const
{
    if (!translucentTrianglesBuffered)
        return;
    glBindBuffer(GL_ARRAY_BUFFER, 0);
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

void Mesh::SetName(uint32_t name)
{
    names.resize(triangles.size(), name);
}

void Mesh::SetColor(RGBA color)
{   
    Degenerate();
    for (auto& v : vertices)
    {
        v.color = color;
    }
}

Mesh& Mesh::operator+=(const Mesh& addition)
{
    Degenerate();

    uint32_t vertexOffset = vertices.size();
    vertices.insert(vertices.end(), addition.vertices.begin(), addition.vertices.end());
    assert(names.empty() == addition.names.empty()); // no functionality to add unnamed to named
    names.insert(names.end(), addition.names.begin(), addition.names.end());
    for (const auto& t : addition.triangles)
    {
        triangles.push_back(Triangle{ t.vertex[0] + vertexOffset, t.vertex[1] + vertexOffset, t.vertex[2] + vertexOffset });
    }
    return *this;
}

Mesh& Mesh::operator*=(const Matrix& transformation)
{
    Degenerate();
    for (auto& vertex : vertices)
    {
        vertex.c *= transformation;
        vertex.n *= transformation.Rotation();
    }
    return *this;
}


void Mesh::Degenerate()
{
    glDeleteBuffers(buffers, buffer);
    ZeroMemory((void*)buffer, buffers*sizeof(uint32_t));
}

template<typename T, typename U> constexpr size_t offsetOf(U T::*member)
{
    return (BYTE*)&((T*)nullptr->*member) - (BYTE*)nullptr;
}

void Mesh::Generate() const
{
    if (vertexBuffer)
        return;

    GenerateVertexBuffer();
    GenerateIndexBuffer();
}

void Mesh::GenerateVertexBuffer() const
{
    glGenBuffers(1, &vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, vertices.size()*sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);
  
    glEnableClientState(GL_VERTEX_ARRAY);

    assert(3 * sizeof(GLdouble) == sizeof(Vertex::c));
    glVertexPointer(3, GL_DOUBLE, sizeof(Vertex), (void*)offsetOf(&Vertex::c));
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glTexCoordPointer(3, GL_DOUBLE, sizeof(Vertex), (void*)offsetOf(&Vertex::t));

    glEnableClientState(GL_NORMAL_ARRAY);
    glNormalPointer(GL_DOUBLE, sizeof(Vertex), (void*)offsetOf(&Vertex::n));

    glEnableClientState(GL_COLOR_ARRAY);
    glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(Vertex), (void*)offsetOf(&Vertex::color));
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Mesh::GenerateIndexBuffer() const
{
/*
    if (!opaqueTriangleBuffer)
    {
        glGenBuffers(1, &opaqueTriangleBuffer);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, opaqueTriangleBuffer);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, triangles.size() * sizeof(Triangle), triangles.data(), GL_STATIC_DRAW);
        opaqueTrianglesBuffered = triangles.size();
    }
*/

    if (!opaqueTriangleBuffer)
    {
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
}

Box::Box(const Vector& size)
    : Box()
{
    (*this) *= Matrix::Scale(size*0.5);
}

Box::Box()
{
    struct Face
    {
        Vector normal;
        Vector diagonal;
        double clock;
    };
    std::array<Face, 6> faces =
    {
        Face{ Vector(1, 0, 0), Vector(0,-1, 1), -0.5 },
        Face{ Vector(0, 1, 0), Vector(1, 0, 1), 0.5 },
        Face{ Vector(0, 0, 1), Vector(-1,-1,0), 0.5 },
        Face{ Vector(-1, 0, 0), Vector(0, 1,-1),-0.5 },
        Face{ Vector(0,-1, 0), Vector(-1, 0,-1), 0.5 },
        Face{ Vector(0, 0,-1), Vector(1, 1, 0), 0.5 },
    };
    std::array<TextureCoordinate, 4> texCoords =
    {
        TextureCoordinate{ 0.0, 0.0, 0.0 },
        TextureCoordinate{ 0.0, 1.0, 0.0 },
        TextureCoordinate{ 1.0, 1.0, 0.0 },
        TextureCoordinate{ 1.0, 0.0, 0.0 }
    };

    vertices.resize(faces.size() * 4);
    triangles.resize(faces.size() * 2);
    uint32_t vi = 0, ti = 0;
    for (const auto & face : faces)
    {
        triangles[ti++] = Triangle{ vi, vi + 1, vi + 3 };
        triangles[ti++] = Triangle{ vi + 1, vi + 2, vi + 3 };

        Coordinate faceCenter = Coordinate::zero + face.normal;
        Vector diagonal = face.diagonal;
        double angle = 0;
        for (const auto& t : texCoords)
        {
            vertices[vi].c = faceCenter - Quaternion(face.normal, angle) * diagonal;
            vertices[vi].n = face.normal;
            vertices[vi].color = RGBA::white;
            // test vertices[vi].color = RGBA(face.normal.x*127+127, face.normal.y*127+127, face.normal.z*127+127);
            vertices[vi].t = t;
            ++vi;
            angle += PI * face.clock; // CCW
        }
    }
}

}

