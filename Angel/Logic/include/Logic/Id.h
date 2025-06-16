#pragma once
#include <string>

namespace Angel::Logic
{

class Id
{
public:
	explicit Id(const std::string_view name="");
    explicit operator bool() const;
	bool operator==(const Id& id) const;
    std::size_t Hash() const;
    operator std::string() const;
private:
    std::string name;
};

std::ostream& operator<<(std::ostream& os, const Id& id);

}