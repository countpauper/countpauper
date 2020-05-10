#pragma once

// NB Named maths.h because otherwise the release build gets confused with stdlib math.h
namespace Engine
{
    double Gaussian(double x, double stddev);
    double FullWidthHalfMaximum(double stddev);
    double Average(double a, double b);
    template<typename T>
    T Sqr(T v) { return v * v; }

    template<class T>
    T Lerp(const T& a, const T& b, double factor)
    {   // linear interpolation. Caller should ensure factor is [0...1]
        return static_cast<T>((a * (1 - factor)) +
            (b * factor));
    }



}