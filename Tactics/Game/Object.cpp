#include "stdafx.h"
#include "Engine/Coordinate.h"
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

	unsigned Object::Tag() const
	{
		static_assert(sizeof(unsigned) == sizeof(this), "Failed to use object pointer as id");
		return unsigned(this);
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