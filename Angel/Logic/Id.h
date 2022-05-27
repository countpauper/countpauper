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
	explicit Id(const std::string& name);
	bool operator==(const Id& id) const;
	bool operator==(const Expression& value) const override;
    Object Compute(const Knowledge& known) const;
protected:
    Object Cast(const std::type_info& t, const Knowledge& k) const override;
private:
    std::string name;
};


Object id(const std::string_view name);

}
}