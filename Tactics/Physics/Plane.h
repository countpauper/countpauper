#pragma once

#include <map>
#include <set>
#include "Game/Direction.h"

namespace Game
{
	class Direction;

	class Plane : public std::set<Direction>
	{
	public:
		Plane() = default;
		explicit Plane(std::initializer_list<Direction> init);
		static const Plane left;
		static const Plane right;
		static const Plane coronal;
		static const Plane front;
		static const Plane back;
		static const Plane sagittal;
		static const Plane around;
		static const Plane top;
		static const Plane bottom;
		static const Plane transversal;
		static const Plane all;
		static const std::map<const std::wstring, Plane> map;
	};

	std::wistream& operator>>(std::wistream& s, Plane& plane);
	std::ostream& operator<<(std::ostream& s, const Plane& plane);
}
