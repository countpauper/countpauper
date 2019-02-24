#pragma once
#include <string>
#include <optional>
#include "Object.h"
#include "Element.h"

namespace Angel
{
namespace Logic
{

class Integer : public Element
{
public:
	explicit Integer(long value);
	bool operator==(const Integer& value) const;
	bool operator==(const Item& value) const override;
	long operator*() const;
	static std::optional <long> Parse(const std::wstring& tag);
private:
	long value;
};


Object integer(long value);

}
}