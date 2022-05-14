#pragma once
#include <string>
#include <optional>
#include "Object.h"
#include "Element.h"

namespace Angel
{
namespace Logic
{

// An integer is a whole number element, the size depends on the CPU type
class Integer : public Element
{
public:
	explicit Integer(long value);
	bool operator==(const Integer& value) const;
	bool operator==(const Expression& value) const override;
	long operator*() const;
	static std::optional <long> Parse(const std::wstring& tag);
    Object Compute(const Knowledge& known) const;
protected:
    Object Cast(const std::type_info& t, const Knowledge& k) const override;
private:
	long value;
};


Object integer(long value);

}
}