#include "stdafx.h"
#include "Geometry.h"

namespace Engine
{
    double ShortestTurn(float a, float b)
    {
        float result = a - b;
        if (result > M_PI)
        {
            return -2 * M_PI + result;
        }
        else if (result < -M_PI)
        {
            return 2 * M_PI + result;
        }
        return result;
    }

    double Rad2Deg(double rad)
    {
        return (rad / M_PI)*180.0f;
    }

}

