#pragma once

#include "Element.h"

namespace Angel
{
namespace Logic
{

	class Boolean : public Value
	{
	public:
		explicit Boolean(bool v);
		bool operator==(const Value& other) const override;
	private:
		bool truth;
	};

	Element boolean(bool v);
}
}