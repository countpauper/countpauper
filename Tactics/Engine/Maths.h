#pragma once

#include "Vector.h"
#include <type_traits>
#include <functional>
#include <limits>
#include <math.h>

// NB Named maths.h because otherwise the release build gets confused with stdlib math.h
namespace Engine
{
    double Gaussian(double x, double stddev);
    double InvGaussian(double y, double stddev);    // always positive

    double FullWidthHalfMaximum(double stddev);
    
    double Average(double a, double b);

    template<typename T>
    T Sqr(T v) { return v * v; }

    // linear interpolation if factor : [0..1[
    template<class T>
    T Lerp(const T& a, const T& b, double x)
    {  
        return static_cast<T>((a * (1 - x)) +
            (b * x));
    }

    template<class T>
    T Lerp(double av, const T& a, double bv, const T& b, double x)
    {
        return Lerp(a, b, (x - av) / (bv - av));
    }


    template<class T>
    T BilinearInterpolation(const T d[4], double x, double y)
    {   // d is column major, ie (d[0,0], d[0,1], d[1,0], d[1,1])
        auto d0 = Lerp(d[0], d[1], y);
        auto d1 = Lerp(d[2], d[3], y);
        return Lerp(d0, d1, x);
    }

    template<class T>
    T TrilinearInterpolation(const T d[8], const Engine::Vector& v)
    {   // d is column major, ie (d[0,0,0], d[0,0,1], d[0,1,0] ... d[1,1,1])
        auto x0 = BilinearInterpolation(d, v.y, v.z);
        auto x1 = BilinearInterpolation(d + 4, v.y, v.z);
        return Lerp(x0, x1, v.x);
    }

    // Logistic sigmoid 0...1 with f(0)=0.5
    double Sigmoid(double x);

    template<class T>
    T Clip(T v, T min, T max)
    {
        return std::min<T>(std::max<T>(v, min), max);
    }

/**/
    template<typename T>
    T ALittleMore(const typename std::enable_if<std::is_integral<T>::value, T>::type v)
    {
        if (v >= std::numeric_limits<T>::max())
            throw std::range_error("Can't compute a little more than max");
        return v + 1;
    }

    template<typename T>
    T ALittleLess(const typename std::enable_if<std::is_integral<T>::value, T>::type v)
    {
        if (v <= std::numeric_limits<T>::lowest())
            throw std::range_error("Can't compute a little less than the minimum integer");
        return v - 1;
    }

    template<typename T>
    T ALittleMore(const typename std::enable_if<std::is_floating_point<T>::value, T>::type v)
    {
        if (v >= std::numeric_limits<T>::max())
            throw std::range_error("Can't compute a little more than max float");
        return std::nexttoward(v, std::numeric_limits<T>::max());
    }

    template<typename T>
    T ALittleLess(const typename std::enable_if<std::is_floating_point<T>::value, T>::type v)
    {
        if (v <= std::numeric_limits<T>::lowest())
            throw std::range_error("Can't compute a little less than the minimum floating point");
        return std::nexttoward(v, std::numeric_limits<T>::lowest());
    }
    
    template<typename T>
    T RungeKutta(double tn, T yn, double h, std::function<T(double t, const T& state)> df)
    {   // https://en.wikipedia.org/wiki/Runge%E2%80%93Kutta_methods
        T k1 = df(tn,           yn);
        T k2 = df(tn + (h / 2), yn + k1 * (h / 2));
        T k3 = df(tn + (h / 2), yn + k2 * (h / 2));
        T k4 = df(tn + h,       yn + k3 * h);

        return yn + (k1 + (k2 * 2.0) + (k3 * 2.0) + k4) *  (h / 6.0);
    }
}