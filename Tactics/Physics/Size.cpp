#include "stdafx.h"
#include "Physics/Size.h"

namespace Physics
{
Size operator+(const Size& a, const Size& b)
{
    Size o = a;
    o.x += b.x;
    o.y += b.y;
    o.z += b.z;
    return o;
}

Size operator+(const Size& a, int s)
{
    return a + Size(s, s, s);
}

} // ::Game