#include "Engine/Geometry/Size.h"

namespace Engine
{
Size operator+(Size a, Size b)
{
    Size o = a;
    o.x += b.x;
    o.y += b.y;
    o.z += b.z;
    return o;
}

Size operator+(Size a, int s)
{
    return a + Size(s, s, s);
}

}
