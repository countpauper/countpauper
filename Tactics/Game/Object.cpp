#include "stdafx.h"
#include "Engine/Coordinate.h"
#include "Engine/Utils.h"
#include "Object.h"

namespace Game
{
	Object::Object()
	{
	}
	void Object::Move(int dx, int dy)
	{
		position.x = std::min(std::max(0, static_cast<int>(position.x) + dx), 5);
		position.y = std::min(std::max(0, static_cast<int>(position.y) + dy), 5);
	}

    bool Object::Prone() const
	{
		return true;
	}


	Position Object::GetPosition() const
	{
		return position;
	}
	std::wstring Object::Description() const
	{
		return name;
	}


} // ::Game