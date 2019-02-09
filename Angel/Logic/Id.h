#pragma once
#include <string>

namespace Angel
{
namespace Logic
{

class Id
{
public:
	explicit Id(const std::wstring& name);
	bool operator==(const Id& id) const;
private:
    std::wstring name;
};

}
}