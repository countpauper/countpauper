#pragma once

// NB Named maths.h because otherwise the release build gets confused with stdlib math.h
namespace Engine
{
    double Gaussian(double x, double stddev);
    double FullWidthHalfMaximum(double stddev);
    double Average(double a, double b);
}