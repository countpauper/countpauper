#include "stdafx.h"
#include "Math.h"

namespace Engine
{

    double Gaussian(double x, double stddev)
    {
        return exp((x * x) / (-2.0 * stddev * stddev));
    }
}
