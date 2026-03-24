#include "Geometry/RightAngle.h"
#include "Geometry/Angles.h"

namespace Engine
{

RightAngle::RightAngle() = default;

RightAngle::RightAngle(int value) : value(value)
{
}

bool RightAngle::operator==(const RightAngle& other) const
{
	return value == other.value;
}

RightAngle& RightAngle::operator+=(const RightAngle& other)
{
	value += other.value;
	return *this;
}

RightAngle operator+(const RightAngle& a, const RightAngle& b)
{
	RightAngle o(a);
	o += b;
	return o;
}

RightAngle& RightAngle::operator-=(const RightAngle& other)
{
	value -= other.value;
	return *this;
}
RightAngle operator-(const RightAngle& a, const RightAngle& b)
{
	RightAngle o(a);
	o -= b;
	return o;
}

RightAngle RightAngle::operator-() const
{
    return RightAngle(-value);
}

double RightAngle::Angle() const
{
	return static_cast<double>(value) * 0.5 * Engine::PI;
}

RightAngle& RightAngle::Normalize()
{
	value %= 4;
	if (value > 2)
		value -= 4;
	if (value < -1)
		value += 4;
	return *this;
}

}
