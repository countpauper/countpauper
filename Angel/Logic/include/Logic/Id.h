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
	explicit Id(const std::string_view name);
    operator bool() const override;
	bool operator==(const Id& id) const;
	bool operator==(const Expression& value) const override;
    std::size_t Hash() const override;
protected:
    Object Cast(const std::type_info& t, const Knowledge& k) const override;
private:
    friend std::ostream& operator<<(std::ostream& os, const Id& id);
    std::string name;
};

std::ostream& operator<<(std::ostream& os, const Id& id);

Object id(const std::string_view name);

}
}