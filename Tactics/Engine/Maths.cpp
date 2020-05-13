#include "stdafx.h"
#include "Maths.h"

namespace Engine
{

    double Gaussian(double x, double stddev)
    {
        return exp((x * x) / (-2.0 * stddev * stddev));
    }

    double FullWidthHalfMaximum(double stddev)
    {
        static const double fwhm = sqrt(2 * log(2));
        return fwhm * stddev;
    }

    double Average(double a, double b)
    {
        return (a + b)*0.5;
    }

    double Sqr(double v) 
    { 
        return v * v; 
    }

    double Sigmoid(double x)
    {
        return 1.0 / (exp(-x)+ 1.0);
    }
}
