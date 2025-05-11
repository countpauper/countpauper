#pragma once
#include <string>

namespace Angel
{
namespace Logic
{

class Id
{
public:
	explicit Id(const std::string_view name);
    operator bool() const;
	bool operator==(const Id& id) const;
    std::size_t Hash() const;
protected:
    // Object Cast(const std::type_info& t, const Knowledge& k) const;
private:
    friend std::ostream& operator<<(std::ostream& os, const Id& id);
    std::string name;
};

std::ostream& operator<<(std::ostream& os, const Id& id);

// Object id(const std::string_view name);

}
}