#pragma once
#include <string>
#include "Element.h"

namespace Angel
{
namespace Logic
{

class Id : public Value
{
public:
	explicit Id(const std::wstring& name);
	bool operator==(const Id& id) const;
	bool operator==(const Value& value) const override;
private:
    std::wstring name;
};


Element id(const std::wstring& name);

}
}