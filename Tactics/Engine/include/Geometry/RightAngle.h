#pragma once

namespace Engine
{

class RightAngle
{
public:
	RightAngle();
	explicit RightAngle(int value=0);
	bool operator==(const RightAngle& other) const;
	RightAngle& operator+=(const RightAngle& other);
	RightAngle& operator-=(const RightAngle& other);
    RightAngle operator-() const;
	double Angle() const;
    inline operator double() const { return Angle(); }
	RightAngle& Normalize();

    static const RightAngle straight;
    static const RightAngle clockwise;
    static const RightAngle counter_clockwise;
    static const RightAngle around;
private:
	int value;
};
RightAngle operator+(const RightAngle& a, const RightAngle& b);
RightAngle operator-(const RightAngle& a, const RightAngle& b);


}
