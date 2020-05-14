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

Mesh::Mesh()
{
        ZeroMemory((void*)buffer, buffers * sizeof(uint32_t));
}

Mesh::~Mesh()
{
    Degenerate();
}

void Mesh::Render() const
{
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

    glEnableClientState(GL_VERTEX_ARRAY); 
    glEnableClientState(GL_NORMAL_ARRAY); 
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);

    assert(3 * sizeof(GLdouble) == sizeof(Vertex::c));
    glVertexPointer(3, GL_DOUBLE, sizeof(Vertex), &vertices.front().c);
    glTexCoordPointer(2, GL_DOUBLE, sizeof(Vertex), &vertices.front().t);
    glNormalPointer(GL_DOUBLE, sizeof(Vertex), &vertices.front().n);
    glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(Vertex), &vertices.front().color);

    /*assert(sizeof(Triangle) == 3 * sizeof(GLuint));
    for (int i = 0; i < triangles.size(); ++i)
    {
        glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, &triangles.at(i));
    }
    */
    glDrawElements(GL_TRIANGLES, 3*triangles.size(), GL_UNSIGNED_INT, triangles.data());

/*
glBegin(GL_TRIANGLES);
    for (auto t : triangles)
    {
        for (int i = 0; i < 3; ++i)
        {
            const auto& v = vertices.at(t.vertex[i]);
            glNormal3d(v.n.x, v.n.y, v.n.z);
            glTexCoord2d(v.t.s, v.t.t);
            glColor4ub(v.color.r, v.color.g, v.color.b, v.color.a);
            glVertex3d(v.c.x, v.c.y, v.c.z);
        }
    }
    glEnd();
*/
}

void Mesh::RenderTranslucent() const
{
    //   glDrawElements(GL_TRIANGLES, translucent.size(), GL_UNSIGNED_INT, translucent.data());
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

Mesh& Mesh::operator*=(const Matrix& transformation)
{
    for (auto& vertex : vertices)
    {
        vertex.c *= transformation;
    }
    return *this;
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
        TextureCoordinate{ 0.0, 0.0 },
        TextureCoordinate{ 0.0, 1.0 },
        TextureCoordinate{ 1.0, 1.0 },
        TextureCoordinate{ 1.0, 0.0 }
    };

    vertices.resize(faces.size() * 4);
    triangles.resize(faces.size() * 2);
    opaque.resize(triangles.size());
    uint32_t vi = 0, ti = 0;
    for (const auto & face : faces)
    {
        Coordinate faceCenter = Coordinate::zero + face.normal;
        Vector diagonal = face.diagonal;
        triangles[ti++] = Triangle{ vi, vi + 1, vi + 3 };
        triangles[ti++] = Triangle{ vi + 1, vi + 2, vi + 3 };
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

bool Mesh::IsGenerated() const
{
    return buffer[0] != 0;
}

void Mesh::Degenerate()
{
    if (!IsGenerated())
        return;
    glDeleteBuffers(buffers, buffer);
    ZeroMemory((void*)buffer, buffers*sizeof(uint32_t));
}

void Mesh::Generate()
{
    ZeroMemory((void*)buffer, buffers * sizeof(uint32_t));
    glGenBuffers(1, &triangleBuffer);
    glGenBuffers(1, &vertexBuffer);
}

Box::Box(const Vector& size)
    : Box()
{
    (*this) *= Matrix::Scale(size*0.5);
}

}

