#include "stdafx.h"
#include "Geometry.h"
#include "Math.h"
namespace Engine
{
    double ShortestTurn(float a, float b)
    {
        float result = a - b;
        if (result > PI)
        {
            return -2 * PI + result;
        }
        else if (result < -PI)
        {
            return 2 * PI + result;
        }
        return result;
    }

    double Rad2Deg(double rad)
    {
        return (rad / PI)*180.0f;
    }

}

