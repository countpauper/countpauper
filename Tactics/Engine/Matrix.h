#pragma once

namespace Engine
{

struct Vector;

struct Matrix
{
    Matrix() = default;
    Matrix(const Matrix&) = default;
    // Column major matrix { } initialization  
    Matrix(const std::initializer_list<double>& v);
    Matrix(const Vector& x, const Vector& y, const Vector& z, const Vector& t);

    Matrix& operator=(const Matrix&) = default;

    double m[4][4];

    const double& data() const;

    const double* operator[](unsigned row) const
    {
        return &m[row][0];
    }
    double* operator[](unsigned row)
    {
        return &m[row][0];
    }

    Vector X() const;
    Vector Y() const;
    Vector Z() const;
    Vector Scale() const;
    Vector Translation() const;
    void SetTranslation(const Vector& translation);
    Matrix Rotation() const;
    bool IsAffine() const;
    bool IsIdentity() const;
    Matrix Inverse() const;
    Matrix NormalScale() const;
    Matrix& operator*=(const Matrix& o);
    static Matrix Projection();
    static Matrix Identity();
    static Matrix Scale(const Vector& scale);
    static Matrix Translation(const Vector& vector);
    static Matrix Perspective(double near, double far);
};

Matrix operator*(const Matrix& r, const Matrix& l);
Vector operator*(const Matrix& m, const Vector& v);

} // ::Engine
