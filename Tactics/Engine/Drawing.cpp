#include "stdafx.h"
#include "Drawing.h"
#include "Geometry.h"
#include "Quaternion.h"
#include "Vector.h"
#include "Matrix.h"
#include <gl/GL.h>

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

        Vector o = m.X().Normal()* v.x + m.Y().Normal() * v.y + m.Z().Normal() * v.z;
        return o;
/*
        GLfloat matrix[4][4];

        glGetFloatv(GL_PROJECTION_MATRIX, &matrix[0][0]);
        Vector result;
        result.x = v.x * matrix[0][0] + v.y * matrix[0][1] + v.z * matrix[0][2];
        result.y = v.x * matrix[1][0] + v.y * matrix[1][1] + v.z * matrix[1][2];
        result.z = v.x * matrix[2][0] + v.y * matrix[2][1] + v.z * matrix[2][2];
        return result;
 */
    }

    void glVertex(const Vector& v)
    {
        glVertex3d(v.x, v.y, v.z);
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
        Vector cameraFacing = TransformBillboard(Vector(0, 0, 1)).Normal();
        Vector leftArrow = v - Quaternion(cameraFacing, angle) * (v * size);
        Vector rightArrow = v - Quaternion(cameraFacing, -angle) * (v * size);
        glBegin(GL_LINES);
            glVertex3d(0, 0, 0);
            glVertex3d(v.x, v.y, v.z);

            glVertex3d(v.x, v.y, v.z);
            glVertex3d(leftArrow.x, leftArrow.y, leftArrow.z);

            glVertex3d(v.x, v.y, v.z);
            glVertex3d(rightArrow.x, rightArrow.y, rightArrow.z);
        glEnd();


    }
}

