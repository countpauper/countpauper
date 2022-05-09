#pragma once

#include <optional>
#include "Object.h"
#include "Element.h"

namespace Angel
{
namespace Logic
{

// Boolean is an element that is simply true, or false
class Boolean : public Element
{
public:
	explicit Boolean(bool v);
	bool operator==(const Item& other) const override;
	bool operator*() const;
	static std::optional<bool> Parse(const std::wstring& tag);
private:
	bool truth;
};

Object boolean(bool v);
}
}