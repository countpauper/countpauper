#pragma once
#include <string>
#include "Object.h"
#include "Element.h"

namespace Angel
{
namespace Logic
{

class Id : public Element
{
public:
	explicit Id(const std::wstring& name);
	bool operator==(const Id& id) const;
	bool operator==(const Item& value) const override;
private:
    std::wstring name;
};


Object id(const std::wstring& name);

}
}