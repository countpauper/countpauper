#pragma once

namespace Angel
{
namespace Logic
{

class Knowledge;

// An item is the base class of all knowledge
class Item
{
public:
	Item() = default;
	virtual ~Item() = default;
	virtual bool operator==(const Item& other) const = 0;
	bool operator!=(const Item& other) const { return !operator==(other); }
	virtual bool Match(const Item& other, const Knowledge& knowledge) const;
};

}
}