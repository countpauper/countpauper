#include "stdafx.h"
#include "Physics/Plane.h"
#include <sstream>
#include "Engine/from_string.h"
#include "Physics/Direction.h"

namespace Physics
{

	Plane::Plane(std::initializer_list<Direction> init) :
		std::set<Direction>(init)
	{
	}

	const Plane Plane::left({ Direction::left });
	const Plane Plane::right({ Direction::left });
	const Plane Plane::coronal({ Direction::left, Direction::right });
	const Plane Plane::front({ Direction::forward });
	const Plane Plane::back({ Direction::backward });
	const Plane Plane::sagittal({ Direction::forward, Direction::backward });
	const Plane Plane::around({ Direction::left, Direction::forward, Direction::right, Direction::backward });
	const Plane Plane::top({ Direction::up });
	const Plane Plane::bottom({ Direction::down });
	const Plane Plane::transversal({ Direction::up, Direction::down });
	const Plane Plane::all({ Direction::left, Direction::forward, Direction::right, Direction::backward, Direction::up, Direction::down });

	const std::map<const std::wstring_view, Plane> Plane::map =
	{
		{ L"Left", Plane::left},
		{ L"Right", Plane::right },
		{ L"Front", Plane::front},
		{ L"Back", Plane::back},
		{ L"Top", Plane::top },
		{ L"Bottom", Plane::bottom },
		{ L"Coronal", Plane::coronal },
		{ L"Sagittal", Plane::sagittal },
		{ L"Transversal", Plane::transversal},
		{ L"Around", Plane::around },
		{ L"All", Plane::all}
	};

	std::wistream& operator>>(std::wistream& s, Plane& plane)
	{
		std::wstring label;
		s >> label;
		plane = Plane::map.at(label);
		return s;
	}

	std::ostream& operator<<(std::ostream& s, const Plane& plane)
	{
		auto it = std::find_if(Plane::map.begin(), Plane::map.end(), [plane](const decltype(Plane::map)::value_type& plane_pair)
		{
			return plane_pair.second == plane;
		});
		assert(it != Plane::map.end()); // plane not in map
		s << it->first.data();
		return s;
	}

}