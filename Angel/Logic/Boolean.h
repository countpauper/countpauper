#pragma once

#include "Object.h"
#include "Element.h"

namespace Angel
{
namespace Logic
{

	class Boolean : public Element
	{
	public:
		explicit Boolean(bool v);
		bool operator==(const Item& other) const override;
	private:
		bool truth;
	};

	Object boolean(bool v);
}
}