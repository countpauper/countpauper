#include "Rendering/Drawing.h"
#include "Geometry/Angles.h"
#include "Geometry/Quaternion.h"
#include "Geometry/Vector.h"
#include "Geometry/Matrix.h"
#include <GL/gl.h>

namespace Engine
{


    Vector TransformBillboard(const Vector & c)
    {
        auto m = Matrix::Projection();
        return TransformBillboard(c, m);
    }

    Vector TransformBillboard(const Vector &v, const Matrix& m)
    {
        //m.SetTranslation(Vector());
        //Vector o = m.Inverse() * v;

        Vector o = m.X().Normal()* v.X() + m.Y().Normal() * v.Y() + m.Z().Normal() * v.Z();
        return o;
/*
        GLfloat matrix[4][4];

        glGetFloatv(GL_PROJECTION_MATRIX, &matrix[0][0]);
        Vector result;
        result.x = vx * matrix[0][0] + vy * matrix[0][1] + v.z * matrix[0][2];
        result.y = vx * matrix[1][0] + vy * matrix[1][1] + v.z * matrix[1][2];
        result.z = vx * matrix[2][0] + vy * matrix[2][1] + v.z * matrix[2][2];
        return result;
 */
    }

    void glVertex(const Coordinate& c)
    {
        glVertex3d(c.X(), c.Y(), c.Z());
    }

    void glVertex(const Vector& v)
    {
        glVertex3d(v.X(), v.Y(), v.Z());
    }

    void glBillBoard(double width, double height)
    {
        auto a = TransformBillboard(Vector(0, 0, 0));
        auto b = TransformBillboard(Vector(width, 0, 0));
        auto c = TransformBillboard(Vector(width, height, 0));
        auto d = TransformBillboard(Vector(0, height, 0));
        glBegin(GL_QUADS);
            glVertex(a);
            glVertex(b);
            glVertex(c);
            glVertex(d);
        glEnd();
    }

    void glDrawArrow(const Vector& v, double size, double angle)
    {
        Vector cameraFacing = TransformBillboard(Vector::XAxis.Normal());
        Vector leftArrow = v - Quaternion(cameraFacing, angle) * (v * size);
        Vector rightArrow = v - Quaternion(cameraFacing, -angle) * (v * size);
        glBegin(GL_LINES);
            glVertex3d(0, 0, 0);
            glVertex(v);

            glVertex(v);
            glVertex(leftArrow);

            glVertex(v);
            glVertex(rightArrow);
       glEnd();


    }
}

