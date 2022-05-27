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
	bool operator==(const Expression& other) const override;
	bool operator*() const;
    Object Compute(const Knowledge& known) const;
	static std::optional<bool> Parse(const std::string& tag);
protected:
    Object Cast(const std::type_info& t, const Knowledge& k) const override;
private:
	bool truth;
};

Object boolean(bool v);
}
}